// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    18 Jun 2015

#include <TServerSocket.h>
#include <TSystem.h>
#include <TThread.h>
#include "TROOT.h"
#include <THtml.h>
#include <TPad.h>

#include "TVMEMonitor.h"

TServerSocket *TVMEMonitor::fServSock = 0;

ClassImp(TVMEMonitor)

namespace {
  class THtmlThreadInfo2 {
  public:
    THtmlThreadInfo2(THtml* html, bool force): fHtml(html), fForce(force) {}
    Bool_t GetForce() const {return fForce;}
    THtml* GetHtml() const {return fHtml;}

  private:
    THtml* fHtml;
    Bool_t fForce;
  };
}

//______________________________________________________________________________
TVMEMonitor::TVMEMonitor()
: TObject(),
  fDaco(0)
{
  // Default constructor
}
//______________________________________________________________________________
TVMEMonitor::~TVMEMonitor()
{
  // Destructor
  SafeDelete(fServSock);
}
//______________________________________________________________________________
void TVMEMonitor::InitServer(Int_t port)
{
  if (fServSock) {
    Warning("InitServer", "server is already running");
    return;
  }

  fServSock = new TServerSocket(port, kTRUE, 10, -1);
  if (!fServSock || !fServSock->IsValid()) {
    Error("InitServer", "unable to create server socket");
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

  // !!!!!!!!!!!!!!
  // gSystem->InnerLoop();
  // toto je v monitor select
  // https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=14581
  // monitor a connect na ready
  // !!!!!!!!!!!!!!!!

  // kedze acceptujem stale iba jeden socket (je iba jeden client) v tomto pripade (asi) nepotrebujem TMonitor
  // if (!fServ->IsValid())     gSystem->Exit(1);

  //  SEE   Bool_t TFastCgi::Create(const char *args) presne to iste co robim ja !!!!!

  // TTreeCacheUnzip
  // TFilePrefetch::ThreadStart()       nie cez void ALE (TThread::VoidRtnFunc_t)

  // The TTree objects contains both static and transient state. In first approximation you can not use
  // the *same* TTree object from two different thread has they will be stepping on each other (i.e. reading
  // entry n will update the 'single' internal buffer to show the value of entry n, and if *at the same time*
  // you read entry m the 'single' internal buffer will be updated to show the other values ... usually inconsistently).
  //
  // To do what you want, you can use ProofLite (which will use separate process to access the file on disk) or
  // you *must* use several instance of the TFile and TTree so that only one thread access each TTree instance at once.
  //
  //  Cheers,
  //  Philippe.

  TObject o;
  THtmlThreadInfo2 hti(0, false);
  gSystem->Load("libThread");
  TThread* thread = new TThread(ttt, this);
  //TThread *t1 = new TThread(ttt, &hti);//, (void*) 1);
  thread->Run(); // CHECK CO VRACIA !!!!
  // thread->Run(this); // cez this mozno predat argumenty
}

void *TVMEMonitor::ttt(void *args)
{
  // https://root.cern.ch/phpBB2/viewtopic.php?f=3&t=9594&start=30
  // gui_thread2.C

  TVMEMonitor *mon = (TVMEMonitor *)args;
  mon->daco();

  // cez args pointer si predat file, tree, can a pod !!!!

  Printf("ttt1");
  RecvData();
  Printf("ttt2"); // NIKDY !!!!
  return 0;
}

//______________________________________________________________________________
void TVMEMonitor::RecvData()
{
  // pozor medzi init (bind) a volanim recvdata je uz connect, t.j. naraz pride viac accept

  if (!fServSock) {
    //Warning("Receiving", "first InitServer");
    return;
  }

  TSocket *sock;
  Int_t nrecv;
  char buf[2720];

  while (kTRUE) {
    //TThread::Lock(); vobec netsuim naco treba
    Printf("cakam tid = %ld, pid = %d, self=%p", TThread::SelfId(), gSystem->GetPid(), (void *)TThread::Self());
    sock = fServSock->Accept();
    if (!sock || !sock->IsValid()) {
      //Warning("RecvData", "accept failed");
      SafeDelete(sock);
      continue;
    }
    Printf("dockal som sa");

    nrecv = sock->RecvRaw(buf, sizeof(buf));
    Printf("from client received %d bytes: %s\n", nrecv, buf);

    delete sock; // close socket

    TVirtualPad *p = (TVirtualPad *)gROOT->GetListOfCanvases()->At(0);
    if (p) {
      Printf("updatujem");
      p->Modified();
      p->Update();
    }
    else Printf("NENI PAD");
    //TThread::UnLock();
  }
}


void TVMEMonitor::daco() // to je stale v thread
{
  Printf("daco tid = %ld, pid = %d, self=%p", TThread::SelfId(), gSystem->GetPid(), (void *)TThread::Self());
  Printf("gSystem->Sleep(1000)");
  gSystem->Sleep(1000);
}
