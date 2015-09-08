// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    08 Sep 2015

#include <TServerSocket.h>
#include <TThread.h>
#include <TStopwatch.h>

#include "TVMEServer.h"

ClassImp(TVMEServer)

//______________________________________________________________________________
TVMEServer::TVMEServer()
: TObject(),
  fServSock(0),
  fThread(0),
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
  if (fThread) {
    if (fThread->Kill() != 0) Info("Destructor", "thread kill with error");
    delete fThread;
    fThread = 0;
  }

  SafeDelete(fServSock);
}
//______________________________________________________________________________
void TVMEServer::Print(Option_t * /*option*/) const
{
  if (fServSock) fServSock->Print();
  else Printf("no fServSock");

  if (fThread) fThread->Print();
  else Printf("no fThread");
  TThread::Ps();
}
//______________________________________________________________________________
void TVMEServer::CloseServer()
{
  if (!fServSock) return;
  TSocket s(fServSock->GetLocalInetAddress(), fServSock->GetLocalPort());
  if (s.IsValid()) s.SendRaw("", 0); // terminate sock = fServSock->Accept();

  if (fThread) {
    if (fThread->Kill() != 0) Warning("CloseServer", "thread kill with error");
    delete fThread;
    fThread = 0;
  }

  SafeDelete(fServSock);
}

//______________________________________________________________________________
void TVMEServer::OpenServer(Int_t port)
{
  // init socket and start thread which will receive data (in background)

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

  if (fThread) {
    Warning("OpenServer", "server thread already exists");
    return;
  }

  fThread = new TThread("VMEServerThread", ThreadRecvData, (void *)this);
  if (fThread->Run() != 0) Info("OpenServer", "thread run with error");
}
//______________________________________________________________________________
void *TVMEServer::ThreadRecvData(void *ptr)
{
  if (!ptr) return 0;

  TVMEServer *instance = (TVMEServer *)ptr;  // server instance
  instance->RecvData();

  Printf("server thread is done");
  return ptr;

  // https://root.cern.ch/root/htmldoc/TFastCgi.html
  // https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=18371#p78289     ex14b.tar.gz
  // https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=9594&start=30    gui_thread2.C
  // if (fThread->GetState() == TThread::kRunningState)

  // TThread::SetCancelOn(); // to allow to terminate (kill) the thread
  // fThread->SetCancelAsynchronous();
}
//______________________________________________________________________________
void TVMEServer::RecvData()
{
  // receiving data loop (call immediately after OpenServer)

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
    if (!fServSock) return; // close server (thread)
    totalrecv = 0;
    fdata = 0;
    SafeDelete(sock);
    ResetHeader();

    if (IsVerbose()) {
      PrintTime();
      printf("####### waiting on port %d #######\n", fServSock->GetLocalPort());
    }

    sock = fServSock->Accept();
    if (!fServSock) return; // close server (thread)
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
        //
        //  thread add a lock/unlock mechanism in the loop (e.g. for every canvas update)
        //  TThread::Lock();
        //  VME DECODE
        //  canvas update
        //  TThread::UnLock();
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
