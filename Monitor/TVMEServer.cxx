// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    27 Aug 2015

#include <TServerSocket.h>
#include <TStopwatch.h>

#include "TVMEServer.h"

ClassImp(TVMEServer)

//______________________________________________________________________________
TVMEServer::TVMEServer()
: TObject(),
  fServSock(0),
  fStoreDir(),
  fDataCode(-1),
  fDataFrom(-1),
  fDataTo(-1),
  fDataName()
{
  // Default constructor
}
//______________________________________________________________________________
TVMEServer::~TVMEServer()
{
  // Destructor
  SafeDelete(fServSock);
}
//______________________________________________________________________________
void TVMEServer::OpenServer(Int_t port)
{
  if (fServSock) {
    Warning("OpenServer", "server socket already exists");
    return;
  }

  fServSock = new TServerSocket(port, kTRUE, TServerSocket::kDefaultBacklog, -1);
  if (!fServSock || !fServSock->IsValid()) {
    Error("OpenServer", "unable to create server socket");
    SafeDelete(fServSock);
    return;
  }

  /*
   * TUnixSystem::UnixTcpService()
   * sock = socket(AF_INET, SOCK_STREAM, 0)
   *
   * reuse = kTRUE      => set SO_REUSEADDR socket option
   * tcpwindowsize = -1 => default SO_RCVBUF, SO_SNDBUF socket option
   *
   * struct sockaddr_in server; // obsolete
   * inserver.sin_family = AF_INET
   * inserver.sin_addr.s_addr = htonl(INADDR_ANY)
   * bind(sock, (struct sockaddr*) &inserver, sizeof(inserver))
   *
   * listen(sock, backlog); // backlog = kDefaultBacklog = 10
   */
}
//______________________________________________________________________________
void TVMEServer::RecvData()
{
  // !!! call immediately after OpenServer !!!

  if (!fServSock) {
    Warning("RecvData", "first OpenServer");
    return;
  }

  TSocket *sock = 0;
  Int_t nrecv;
  Long_t totalrecv;
  FILE *fdata;
  char bufHead[280];    // size !!! must be exactly the same !!! as in vmemonitor_client
  char bufData[4096*1]; // 4096 is blocksize in bytes
  static TStopwatch timer;

  while (kTRUE) {
    totalrecv = 0;
    fdata = 0;
    SafeDelete(sock);
    ResetHeader();

    if (IsVerbose()) {
      PrintTime();
      printf("####### waiting on port %d #######\n", fServSock->GetLocalPort());
    }

    sock = fServSock->Accept();
    if (!sock || !sock->IsValid()) {
      Warning("RecvData", "accept failed");
      continue;
    }

    if (IsVerbose()) {
      timer.Start(kTRUE);
      PrintTime();
      printf("accept \t %s:%d\n", sock->GetInetAddress().GetHostName(), sock->GetPort());
    }

    // receive header (first 280 bytes, always fixed length)
    nrecv = sock->RecvRaw(bufHead, sizeof(bufHead));
    if (nrecv != sizeof(bufHead)) {
      Warning("RecvData", "failed receiving header");
      continue;
    }
    DecodeHeader(bufHead, nrecv);
    if (AreDataValid()) fdata = OpenFile();

    // receive data
    while (kTRUE) {
      nrecv = sock->RecvRaw(bufData, sizeof(bufData));
      if (nrecv < 0) {
        Warning("RecvData", "failed receiving data");
        break;
      } else if (nrecv == 0)
        break;

      totalrecv += nrecv; // sock->GetBytesRecv() return only UInt_t (not correct for big data)
      if (fdata) fwrite(bufData, sizeof(char), nrecv, fdata);
    }

    if (fdata) {
      fflush(fdata);
      if (ftell(fdata) != totalrecv) {
        Warning("RecvData", "not wrote all bytes");
        ResetBit(kDataValid);
      }
      if ((fDataTo - fDataFrom) != totalrecv) {
        Warning("RecvData", "not deliver all bytes");
        ResetBit(kDataValid);
      }

      if (AreDataValid()) {
        //  rewind(fdata);
        //  VME DECODE
        fclose(fdata); // temporary
      } else
        fclose(fdata);
    }

    if (IsVerbose()) {
      timer.Stop();
      PrintTime();
      printf("receive bytes \t %ld\n", totalrecv);
      PrintTime(kTRUE);
      printf("receive sec \t %.2f (%6.2f MB/s)\n", timer.RealTime(), (totalrecv/1024./1024.)/timer.RealTime());
      PrintTime(kTRUE);
      printf("valid data \t %s\n", AreDataValid() ? "CORRECT" : "WORNG");
    }
  }
}
//______________________________________________________________________________
void TVMEServer::ResetHeader()
{
  ResetBit(kDataValid);
  fDataCode = -1;
  fDataFrom = -1;
  fDataTo   = -1;
  fDataName = "noname";
}
//______________________________________________________________________________
void TVMEServer::DecodeHeader(const char *buffer, Int_t bsize)
{
  // see vmemonitor_client
  const int headID = 987654321;
  size_t lenbytes;
  int code, headIDbs;
  long datafrom, datato;
  char fname[255 + 1];
  memset(fname, 0, sizeof(fname));

  lenbytes = 0;
  memcpy(&code,     &buffer[lenbytes], sizeof(code));
  lenbytes += sizeof(code);
  memcpy(&datafrom, &buffer[lenbytes], sizeof(datafrom));
  lenbytes += sizeof(datafrom);
  memcpy(&datato,   &buffer[lenbytes], sizeof(datato));
  lenbytes += sizeof(datato);
  memcpy(fname,     &buffer[lenbytes], sizeof(fname));
  lenbytes += sizeof(fname);
  memcpy(&headIDbs, &buffer[lenbytes], sizeof(headIDbs));
  lenbytes += sizeof(headIDbs);

  if (datafrom > datato)
    Info("DecodeHeader", "data from: %ld > data to: %ld\n", datafrom, datato);
  if ((headIDbs - bsize) != headID) {
    Warning("DecodeHeader", "!!! wrong header !!!");
    return;
  }

  SetBit(kDataValid);
  fDataCode = code;
  fDataFrom = datafrom;
  fDataTo   = datato;
  fDataName = fname;

  if (IsVerbose()) {
    PrintTime();
    printf("data fname \t %s (code %d)\n", fDataName.Data(), fDataCode);
    PrintTime(kTRUE);
    printf("data bytes \t %ld -> %ld (total %ld)\n", fDataFrom, fDataTo, fDataTo - fDataFrom);
  }
}
//______________________________________________________________________________
FILE *TVMEServer::OpenFile() const
{
  FILE *f = 0;
  TString fn("tmpfile");

  if (fStoreDir.IsNull())
    f = tmpfile(); // binary read/write (w+b) mode (in /tmp)
  else {
    fn = TString::Format("%s/%s", fStoreDir.Data(), fDataName.Data());
    f = fopen(fn.Data(), "w+b");
  }

  if (!f) {
    Warning("OpenFile", "cannot open %s", fn.Data());
    return 0;
  }

  if (IsVerbose()) {
    PrintTime();
    printf("storage \t %s\n", fn.Data());
  }

  return f;
}
//______________________________________________________________________________
void TVMEServer::PrintTime(Bool_t empty) const
{
  if (empty) {
    printf("         ");
    return;
  }

  TDatime lt;
  printf("%02d:%02d:%02d ", lt.GetHour(), lt.GetMinute(), lt.GetSecond());
}
