// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    11 Nov 2010

#include <TBrowser.h>

#include "TStrelaBase.h"
#include "TStrela.h"

ClassImp(TStrelaBase)

//______________________________________________________________________________
TStrelaBase::TStrelaBase()
{
  //  Info("TStrelaBase", "Default constructor");
  fContainer  = 0;
  fBranchName = "";
}
//______________________________________________________________________________
TStrelaBase::TStrelaBase(const char *name, const char *title) : TNamed(name, title)
{
  //  Info("TStrelaBase", "Normal constructor");
  fContainer  = 0;
  fBranchName = "";

  if (gStrela) gStrela->Detectors()->Add(this);
}
//______________________________________________________________________________
TStrelaBase::~TStrelaBase()
{
  Info("~TStrelaBase", "Destructor");
  if (fContainer && fContainer->InheritsFrom("TClonesArray")) fContainer->Delete();
  delete fContainer;
  fContainer = 0;
  if (gStrela) gStrela->Detectors()->Remove(this);
}
//______________________________________________________________________________
void TStrelaBase::Browse(TBrowser *b)
{
  if (!b || !fContainer) return;

  if (fContainer->InheritsFrom("TClonesArray"))
    for (Int_t ic = 0; ic < GetNumOfContainers(); ic++)
      b->Add(Container(ic));
  else b->Add(fContainer);
}
//______________________________________________________________________________
void TStrelaBase::Init()
{
  if (!gStrela) return;

  TSQLServer *ser = gStrela->ConnectSQL();
  if (!ser) return;

  TList *list = gStrela->Detectors();
  if (ReadSQL(ser)) {
    if (!list->Contains(this)) {
      list->Add(this);
      Info("Init", "add to analyze");
    }
  }
  else if (list->Remove(this))
    // remove only from list, object is always on heap
    Info("Init", "remove from analyze");
}
//______________________________________________________________________________
Bool_t TStrelaBase::ReadSQL(TSQLServer *ser)
{
  // override this function
  return kTRUE;
}
//______________________________________________________________________________
void TStrelaBase::AnalyzeBegin()
{
  // override this function, called before a loop start
}
//______________________________________________________________________________
void TStrelaBase::AnalyzeEntry()
{
  // must override this function, called for each event
  AbstractMethod("AnalyzeEntry");
}
//______________________________________________________________________________
void TStrelaBase::AnalyzeTerminate()
{
  // override this function, called at the end of the loop
}
//______________________________________________________________________________
void TStrelaBase::CreateBranch(TTree *tree)
{
  if (!tree || !fContainer || fBranchName.IsNull()) {
    Error("CreateBranch", "can not create branch");
    return;
  }

  if (fContainer->InheritsFrom("TClonesArray"))
    tree->Branch(GetBranchName(), &fContainer);
  else
    tree->Branch(GetBranchName(), fContainer->ClassName(), &fContainer);
}
