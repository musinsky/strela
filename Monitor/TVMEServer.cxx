// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    28 Nov 2016

#include <TServerSocket.h>
#include <TThread.h>
#include <TStopwatch.h>
#include <THttpServer.h>

#include <TStyle.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TH1.h>
#include <TFolder.h>
#include <TMath.h>
#include <TTree.h>
#include <TROOT.h>
#include <TAxis.h>
#include <TArrayI.h>

#include "TVMEServer.h"
#include "TVMERawData.h"
#include "TStrela.h"
#include "TStrawCham.h"
#include "TStrawTube.h"
#include "THttpMonitor.h"

ClassImp(TVMEServer)

//______________________________________________________________________________
TVMEServer::TVMEServer()
: TObject(),
  fServSock(0),
  fThread(0),
  fHttp(0),
  fStoreDir(),
  fDataCode(-1),
  fDataFrom(-1),
  fDataTo(-1),
  fDataName(),
  fDt(),
  fLg(0)
{
  // Default constructor

  fHttp = new THttpServer("http:7575?top=ALPOM");
  //fHttp->SetReadOnly(kFALSE); //  !! NIE TRUE, nech nie je readonly !!!!!!!!!!!!! dam no exponent a po refreshi sa mi to zmaze
  // treba preskumat ako to funguje

  fHttp->CreateItem("/Log","log output");
  fHttp->SetItemField("/Log", "_kind", "Text");
  fHttp->SetItemField("/Log", "value","log");

  // na konci, resp, az po nejakom SetItem (see TRootSniffer::GetItem)
  // alebo hladat v gROOT->GetListOfCleanups()
  TFolder *topf = gROOT->GetRootFolder();
  TFolder *httpfold = dynamic_cast<TFolder *>(topf->FindObject("http"));
  if (httpfold) httpfold->SetTitle("STRELA monitor");
  // pozor nemenit "name" = "http"

  InitGraph();

  //this._monitoring_on = false;
  //this._monitoring_interval = 3000;
}

//______________________________________________________________________________
void TVMEServer::InitGraph()
{
  if (fLg) {
    fLg->Delete();
    delete fLg;
    fLg = 0 ;
  }
  fLg = new TList();

  fDt.Set();
  gStyle->SetTimeOffset(fDt.Convert());


  for (Int_t i = 0; i < 16; i++) {
    TGraph *g = new TGraph();
    g->SetName(TString::Format("MSC%02d", i+1));
    g->SetTitle(TString::Format("MSC[%02d]", i+1));
    g->SetMarkerStyle(7);
    g->SetLineStyle(3);
    g->SetPoint(0, fDt.Convert() - gStyle->GetTimeOffset(), 0); // first zero point
    fLg->AddAt(g, i);
    fHttp->Register("/", g); // hist automaticky registruje, ale nie graph
  }

  //                                                         "buttton"
  //fHttp->SetItemField("/","_monitoring","3000"); // !!!!!!!!!! by default monitoring !!!!!!!!!

  TMultiGraph *mg = new TMultiGraph("pol", "pol = (L-R)/IC");

  TGraph *g2 = new TGraph();
  g2->SetName("polZ");
  g2->SetTitle("polZ");
  g2->SetMarkerStyle(7);
  g2->SetMarkerColor(kRed);
  g2->SetLineColor(kRed);
  g2->SetLineStyle(3);
  //  g2->SetPoint(0, fDt.Convert() - gStyle->GetTimeOffset(), 0);
  fLg->AddAt(g2, 16);
  mg->Add(g2);

  g2 = new TGraph();
  g2->SetName("polP");
  g2->SetTitle("polP");
  g2->SetMarkerStyle(7);
  g2->SetMarkerColor(kGreen);
  g2->SetLineColor(kGreen);
  g2->SetLineStyle(3);
  //  g2->SetPoint(0, fDt.Convert() - gStyle->GetTimeOffset(), 0);
  fLg->AddAt(g2, 17);
  mg->Add(g2);

  g2 = new TGraph();
  g2->SetName("polM");
  g2->SetTitle("polM");
  g2->SetMarkerStyle(7);
  g2->SetMarkerColor(kBlue);
  g2->SetLineColor(kBlue);
  g2->SetLineStyle(3);
  //  g2->SetPoint(0, fDt.Convert() - gStyle->GetTimeOffset(), 0);
  fLg->AddAt(g2, 18);
  mg->Add(g2);


  fHttp->Register("/", mg); // hist automaticky registruje, ale nie graph


  mg = new TMultiGraph("cham", "small & big");

  g2 = new TGraph();
  g2->SetName("small");
  g2->SetTitle("small");
  g2->SetMarkerStyle(7);
  g2->SetMarkerColor(kBlue);
  g2->SetLineColor(kBlue);
  g2->SetLineStyle(3);
  g2->SetPoint(0, fDt.Convert() - gStyle->GetTimeOffset(), 0);
  fLg->AddAt(g2, 19);
  mg->Add(g2);

  g2 = new TGraph();
  g2->SetName("big");
  g2->SetTitle("big");
  g2->SetMarkerStyle(7);
  g2->SetMarkerColor(kGreen);
  g2->SetLineColor(kGreen);
  g2->SetLineStyle(3);
  g2->SetPoint(0, fDt.Convert() - gStyle->GetTimeOffset(), 0);
  fLg->AddAt(g2, 20);
  mg->Add(g2);

  //  mg->GetXaxis()->SetTimeDisplay(1);
  //  mg->GetXaxis()->SetTimeFormat("%H:%M:%S");
  //  mg->GetXaxis()->SetNdivisions(407);

  fHttp->Register("/", mg); // hist automaticky registruje, ale nie graph

  fHttp->RegisterCommand("/control/Reset", "s->InitGraph()", "rootsys/icons/ed_delete.png");
  // !!! toto je zle, lebo size zmaze grafy, ale znova je tam Register => zaregistrovane zmazane grafy + nove
  // treba len resetovat grafy (asi cez g->Set(0)) a teda netreba znova Register-ovat

  if (gStrela) {
    TIter next(gStrela->StrawCham()->Tubes());
    TStrawTube *tube;
    while ((tube = (TStrawTube *)next())) {
      fHttp->Register("/TDC", tube->HisTime1());
      fHttp->Register("/TDC", tube->HisDelta());
    }
    fHttp->RegisterCommand("/TDC/Reset", "gStrela->HistoManager(\"*\", \"reset\")", "rootsys/icons/ed_delete.png");
  }

  fHttp->SetItemField("/","_monitoring","3000"); // 3000 is default
  fHttp->SetItemField("/","_layout","grid2x2");
  fHttp->SetItemField("/","_drawitem","[Log,MSC07,MSC08,MSC05]");
  //fHttp->SetItemField("/","_drawopt","col");

  // s = new TVMEServer; s->Verbose(0); s->OpenServer();
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
  if (fServSock) {
    Printf("total receive bytes %llu", fServSock->GetSocketBytesRecv());
    Printf("total sent    bytes %llu", fServSock->GetSocketBytesSent());
    fServSock->Print();
  }
  if (fThread) fThread->Print();
  TThread::Ps();
}
//______________________________________________________________________________
void TVMEServer::OpenServer(Int_t port, Bool_t inthread)
{
  // init socket and start thread which will receive data (in background)

  if (fServSock || fThread) {
    Warning("OpenServer", "server socket or thread already exists");
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

  if (inthread) {
    fThread = new TThread("VMEServerThread", ThreadRecvData, (void *)this);
    if (fThread->Run() != 0) Info("OpenServer", "thread run with error");
  } else {
    RecvData();
    Printf("RecvData is done");
  }
}
//______________________________________________________________________________
void *TVMEServer::ThreadRecvData(void *ptr)
{
  // trial thread function (probably not thread safe)

  if (!ptr) return 0;

  TVMEServer *instance = (TVMEServer *)ptr;
  instance->RecvData();
  Printf("ThreadRecvData is done");
  return ptr;
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

  TVMERawData *rd = new TVMERawData();
  rd->Verbose(IsVerbose());

  TString log;
  Int_t spills = 0;
  ULong_t entries = 0;

  Long64_t p_z, p_p, p_m, tt;

  while (kTRUE) {
    totalrecv = 0;
    fdata = 0;
    SafeDelete(sock);
    ResetHeader();

    if (IsVerbose()) {
      PrintTime();
      printf("####### waiting on port %d #######\n", fServSock->GetLocalPort());
    }

    ///////////////////////////    if (gSystem->ProcessEvents()) {Printf("AAAAAAAAAAAAAAAAAAAAAAAA"); break;}
    //fHttp->ProcessRequests(); // bez UPDATE efektu
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
        //  thread add a lock/unlock mechanism in the loop (e.g. for every canvas update)
        //  TThread::Lock();
        //  VME DECODE
        //  CANVAS UPDATE
        //  TThread::UnLock();
        //        fclose(fdata); // temporary
        rd->DecodeMonitor(fdata);
        fclose(fdata);


        if (gStrela) {
          gStrela->ChangeBranchAddress(rd->GetTree());
          //if (IsResetCycle()) gStrela->AnalyzeEntries(0);
          ///else
          gStrela->AnalyzeEntries(0, "hnoreset");
        }



        if (fLg) {
          for (Int_t l = 0; l < 16; l++) { // !!!!!!!!!!!! 16
            TGraph *g = (TGraph *)fLg->At(l);
            fDt.Set();
            g->SetPoint(g->GetN(), fDt.Convert()-gStyle->GetTimeOffset(), rd->GetMSCCnt().At(l));
            g->GetXaxis()->SetTimeDisplay(1);
            g->GetXaxis()->SetTimeFormat("%H:%M:%S");
            g->GetXaxis()->SetNdivisions(407);
          }
        }

        printf("%02d:%02d:%02d \t events: %d", fDt.GetHour(), fDt.GetMinute(), fDt.GetSecond(), rd->GetNEvents());

        entries += rd->GetNEvents();
        fHttp->SetItemField("/Log", "value",
                            TString::Format("total spills:\t %d \ntotal entries:\t %ld \n \nrecv data:\t %s \nrecv bytes:\t %ld -> %ld (total %ld) \nrecv entries:\t %d \nrecv time:\t %02d:%02d:%02d",
                                            ++spills, entries, fDataName.Data(), fDataFrom, fDataTo, totalrecv, rd->GetNEvents(), fDt.GetHour(), fDt.GetMinute(), fDt.GetSecond())
        );


        p_z =  rd->GetTree()->Draw("", "EventTdc.MultiTdc(56)", "goff");
        p_m =  rd->GetTree()->Draw("", "EventTdc.MultiTdc(57)", "goff");
        tt  =  rd->GetTree()->Draw("", "EventTdc.MultiTdc(58)", "goff");
        p_p =  rd->GetTree()->Draw("", "EventTdc.MultiTdc(59)", "goff");

        //        Printf("56 %lld, 57 %lld, 58 %lld, 59 %lld", p_z, p_m, tt, p_p);

        Int_t iig = -1;

        if      (TMath::Abs(tt*0.90 - p_z) < tt*0.11) iig = 16;//Printf("Z");
        else if (TMath::Abs(tt*0.90 - p_m) < tt*0.11) iig = 18;//Printf("M");
        else if (TMath::Abs(tt*0.90 - p_p) < tt*0.11) iig = 17;//Printf("P");
        else                                          iig = -1;//Printf("NO POLAR");

        Double_t left = rd->GetMSCCnt().At(5-1);
        Double_t right = rd->GetMSCCnt().At(6-1);
        Double_t ik = rd->GetMSCCnt().At(7-1);      // check on 0
        Double_t ratio = (left-right)/ik;
        if(rd->GetMSCCnt().At(7-1) == 0) ratio=0.0;
        //        Double_t ratio = (left-right)/(left+right);


        if (iig != -1) {
          TGraph *g = (TGraph *)fLg->At(iig);
          //          fDt.Set();
          g->SetPoint(g->GetN(), fDt.Convert()-gStyle->GetTimeOffset(), ratio);
          g->GetXaxis()->SetTimeDisplay(1);
          g->GetXaxis()->SetTimeFormat("%H:%M:%S");
          g->GetXaxis()->SetNdivisions(407);
          printf("\t pol: %d, ratio: %7f\n", iig, ratio);
        }


        TGraph *g = (TGraph *)fLg->At(19); // small
        g->SetPoint(g->GetN(), fDt.Convert()-gStyle->GetTimeOffset(), rd->GetTree()->Draw("", "GetChannelTdc() < 31", "goff"));
        g->GetXaxis()->SetTimeDisplay(1);
        g->GetXaxis()->SetTimeFormat("%H:%M:%S");
        g->GetXaxis()->SetNdivisions(407);

        g = (TGraph *)fLg->At(20); // big
        g->SetPoint(g->GetN(), fDt.Convert()-gStyle->GetTimeOffset(), rd->GetTree()->Draw("", "GetChannelTdc() > 63", "goff"));
        g->GetXaxis()->SetTimeDisplay(1);
        g->GetXaxis()->SetTimeFormat("%H:%M:%S");
        g->GetXaxis()->SetNdivisions(407);




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
