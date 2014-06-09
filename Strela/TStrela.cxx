// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    09 Jun 2014

#include <TROOT.h>
#include <TSQLServer.h>
#include <TFile.h>
#include <TBrowser.h>
#include <TSystem.h>
#include <TFriendElement.h>
#include <TH1.h>
#include <TRegexp.h>
#include <TStopwatch.h>

#include "TStrela.h"
#include "TPommeEvent.h"
#include "TStrawEvent.h"
#include "TGemEvent.h"
#include "TVMEEvent.h"
#include "TWireCham.h"
#include "TStrawCham.h"
#include "TStrelaDisplay.h"

TStrela *gStrela = 0;

ClassImp(TStrela)

//______________________________________________________________________________
TStrela::TStrela()
{
  //  Info("TStrela", "Default constructor");
  fSeance     = "";
  fPath       = "";
  fChain      = 0;
  fRawEvents  = 0;
  fPommeEvent = 0;
  fStrawEvent = 0;
  fGemEvent   = 0;
  fVMEEvent   = 0;
  fDetectors  = 0;
  fWireCham   = 0;
  fStrawCham  = 0;
  fDisplay    = 0;
  fHistograms = 0;
}
//______________________________________________________________________________
TStrela::TStrela(const char *name, const char *title) : TNamed(name, title)
{
  //  Info("TStrela", "Normal constructor");
  gStrela     = this;
  fSeance     = "";
  fPath       = "";
  fChain      = 0;

  fRawEvents  = new TList();
  // event must be null, create only if exist referenced branch in tree
  fPommeEvent = 0;
  fStrawEvent = 0;
  fGemEvent   = 0;
  fVMEEvent   = 0;

  // order of detectors in list is not important (meanwhile)
  fDetectors  = new TList();
  fWireCham   = 0;
  //  fWireCham   = new TWireCham("WireCham", "Find points in wire chambers");
  //  no more need WireCham (MWPC) detectors
  fStrawCham  = new TStrawCham("StrawCham", "Find tracks in straw chambers");
  fDisplay    = 0;
  fHistograms = new TList();
  gROOT->GetListOfCleanups()->Add(fHistograms);

  gROOT->GetListOfBrowsables()->Add(this, title);
  Printf("\n\033[1m %s\033[0m", title);
  Printf(" Last build: %s  %s\n", __DATE__, __TIME__);
}
//______________________________________________________________________________
TStrela::~TStrela()
{
  Info("~TStrela", "Destructor");
  if (fRawEvents) fRawEvents->Delete();
  delete fRawEvents;
  if (fDetectors) fDetectors->Delete();
  delete fDetectors;
  delete fDisplay;
  delete fHistograms;
  gROOT->GetListOfBrowsables()->Remove(this);
  gStrela = 0;
}
//______________________________________________________________________________
void TStrela::Browse(TBrowser *b)
{
  if (!b) return;
  if (fChain) {
    b->Add(fChain);
    if (fChain->GetListOfFriends()) {
      TIter nextf(fChain->GetListOfFriends());
      TFriendElement *fe;
      while ((fe = (TFriendElement *)nextf()))
        b->Add(fe->GetTree());
    }
  }
  TIter next(fDetectors);
  TStrelaBase *detector;
  while ((detector = (TStrelaBase *)next()))
    b->Add(detector);

  if (!fHistograms->IsEmpty()) b->Add(fHistograms, "histograms");
}
//______________________________________________________________________________
void TStrela::Init() const
{
  TIter next(fDetectors);
  TStrelaBase *detector;
  while ((detector = (TStrelaBase *)next()))
    detector->Init();
}
//______________________________________________________________________________
TSQLServer *TStrela::ConnectSQL() const
{
  // must delete returned SQLServer
  TSQLServer *ser = TSQLServer::Connect("mysql://localhost", "", "");
  if (!ser) return 0;
  if (ser->SelectDataBase(GetSeance()) == 0) return ser;
  delete ser;
  return 0;
}
//______________________________________________________________________________
void TStrela::SetRun(Int_t run)
{
  TFile *file = TFile::Open(Form("%s/run%03d.root", GetPath(), run), "READ");
  if (!file) return;
  if (fChain && fChain->GetCurrentFile()) delete fChain->GetCurrentFile();
  fChain = 0;

  TChain *tree = (TChain *)file->Get("pp");
  if (!tree) tree = (TChain *)file->Get("dp");
  if (!tree) delete file;
  ChangeBranchAddress(tree);
}
//______________________________________________________________________________
void TStrela::ChangeBranchAddress(TChain *chain)
{
  if (!chain) {
    Error("ChangeBranchAddress", "pointer to TTree/TChain is null");
    return;
  }

  fChain = chain;
  // strongly recommend do not manipulate with the TBranch objects directly
  // only this way (for TChain branch->SetAddresss(...) will not work)
  if (fChain->GetBranch("pommes")) {
    fChain->SetBranchAddress("pommes", &fPommeEvent);
    if (!fRawEvents->FindObject(fPommeEvent)) fRawEvents->Add(fPommeEvent);
  }
  if (fChain->GetBranch("straws")) {
    fChain->SetBranchAddress("straws", &fStrawEvent);
    if (!fRawEvents->FindObject(fStrawEvent)) fRawEvents->Add(fStrawEvent);
  }
  if (fChain->GetBranch("event")) {
    fChain->SetBranchAddress("event", &fGemEvent);
    if (!fRawEvents->FindObject(fGemEvent)) fRawEvents->Add(fGemEvent);
  }
  if (fChain->GetBranch("VMEEvent")) {
    fChain->SetBranchAddress("VMEEvent", &fVMEEvent);
    if (!fRawEvents->FindObject(fVMEEvent)) fRawEvents->Add(fVMEEvent);
  }

  if (fChain->InheritsFrom("TChain")) {
    fChain->GetEntries();
    fChain->ls();
  }
  else Printf("Working file %s", fChain->GetCurrentFile()->GetName());
}
//______________________________________________________________________________
void TStrela::AnalyzeEntry(Int_t entry) const
{
  // read complete tree (all branches)
  if (!fChain || (fChain->GetEntry(entry) <= 0)) {
    Error("AnalyzeEntry", "unreadable entry %d", entry);
    return;
  }

  TIter next(fDetectors);
  TStrelaBase *detector;
  while ((detector = (TStrelaBase *)next()))
    detector->AnalyzeEntry();
}
//______________________________________________________________________________
void TStrela::AnalyzeEntries(Int_t ne, Option_t *option, Bool_t timer) const
{
  if (!fChain) {
    Error("AnalyzeEntries", "pointer to TTree/TChain is null");
    return;
  }
  if (GetNumOfDetectors() == 0) {
    Warning("AnalyzeEntries", "first initializing any detectors");
    return;
  }

  TString opt = option;
  opt.ToLower();
  if (opt != "hnoreset") HistoManager("*", "reset");

  static TStopwatch watch;
  if ((ne == 0) || (ne > GetEntries())) ne = GetEntries();
  Printf("Number of analyzing entries = %d", ne);
  watch.Start();

  TIter next(fDetectors);
  TStrelaBase *detector;
  while ((detector = (TStrelaBase *)next()))
    detector->AnalyzeBegin();
  for (Int_t i = 0; i < ne; i++)
    AnalyzeEntry(i);
  next.Reset();
  while ((detector = (TStrelaBase *)next()))
    detector->AnalyzeTerminate();

  watch.Stop();
  if (timer) Printf("Real time = %.2f sec", watch.RealTime());
}
//______________________________________________________________________________
void TStrela::AddFriend(Option_t *option, const char *dirname) const
{
  if (!fChain) {
    Error("AddFriend", "pointer to TTree/TChain is null");
    return;
  }
  if (fChain->InheritsFrom("TChain")) {
    Error("AddFriend", "only for tree (not chain)");
    return;
  }

  TString treeName = Form("%s2", fChain->GetName());
  TString fileName(gSystem->BaseName(fChain->GetCurrentFile()->GetName()));
  fileName.Replace(0, fileName.Index(".")-3, "dst"); // 3 digits
  gSystem->PrependPathName(GetSeance(), fileName);
  gSystem->PrependPathName(dirname, fileName);

  // checking dir
  TString opt = option;
  opt.ToUpper();
  if ((opt == "NEW") || (opt == "RECREATE")) {
    TString dirName = gSystem->DirName(fileName.Data());
    if (gSystem->AccessPathName(dirName.Data()))
      if (gSystem->mkdir(dirName.Data(), kTRUE)) {
        SysError("AddFriend", "dir %s is not writable", dirName.Data());
        return;
      }
  }

  TFile *file = new TFile(fileName.Data(), option);
  if (file->IsZombie()) {
    delete file;
    return;
  }
  Bool_t onlyRead = strcmp(file->GetOption(), "READ") ? kFALSE : kTRUE;

  if (!onlyRead) {
    TTree *treeFriend = new TTree(treeName.Data(), "tracks tree");
    treeFriend->SetAutoSave(1000000000); // autosave when 1 Gbyte, uncompressed

    TIter next(fDetectors);
    TStrelaBase *detector;
    while ((detector = (TStrelaBase *)next()))
      detector->CreateBranch(treeFriend);

    Printf("Number of analyzing entries = %d", GetEntries());
    for (Int_t i = 0; i < GetEntries(); i++) {
      AnalyzeEntry(i);
      treeFriend->Fill();
    }
    treeFriend->Write();
  }

  delete file; // delete in any case
  if (fChain->GetListOfFriends()) {
    TFriendElement *oldFriend = (TFriendElement *)fChain->GetListOfFriends()->FindObject(treeName.Data());
    if (oldFriend) {
      if (onlyRead) {
        Error("AddFriend", "friend element %s already exists", treeName.Data());
        return;
      }
      fChain->RemoveFriend(oldFriend->GetTree());
    }
  }

  fChain->AddFriend(treeName.Data(), fileName.Data());
  fChain->GetListOfFriends()->ls();
}
//______________________________________________________________________________
const char *TStrela::GetEventInfo() const
{
  return Form("event: %lld from %s", fChain->GetReadEntry(), fChain->GetCurrentFile()->GetName());
}
//______________________________________________________________________________
void TStrela::HistoManager(TH1 *his, Option_t *option) const
{
  if (!his) return;
  TString action = option;
  action.ToLower();

  if (action == "add") {
    TH1 *prev = His(his->GetName()); // find previous histo with same name
    if (prev) {
      Warning("HistoManager", "%s replaced", prev->GetName());
      delete prev; // and remove from fHistograms
    }
    his->SetDirectory(0);
    his->SetBit(kMustCleanup);
    fHistograms->Add(his);
  }

  if (action == "reset") his->Reset();
}
//______________________________________________________________________________
void TStrela::HistoManager(const char *wildcard, Option_t *option) const
{
  if (!wildcard) wildcard = "";
  TRegexp re(wildcard, kTRUE);
  Int_t nch = strlen(wildcard);

  TIter next(fHistograms);
  TH1 *histo;
  while ((histo = (TH1 *)next())) {
    TString s = histo->GetName();
    if (nch && s != wildcard && s.Index(re) == kNPOS) continue;
    if (gDebug > 0) Printf("%s", histo->GetName());
    HistoManager(histo, option);
  }
}
