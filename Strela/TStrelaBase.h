// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 16 Apr 2008

#ifndef STRELA_TStrelaBase
#define STRELA_TStrelaBase

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif
#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif

class TSQLServer;
class TTree;

class TStrelaBase : public TNamed {

protected:
  TObject      *fContainer;  //  pointer to container(hits)
  TString       fBranchName; //  name of branch

public:
  TStrelaBase();
  TStrelaBase(const char *name, const char *title);
  virtual      ~TStrelaBase();

  TObject      *Container() const { return fContainer; }
  const char   *GetBranchName() const { return fBranchName.Data(); }
  TClonesArray *Containers() const { return (TClonesArray *)fContainer; }
  Int_t         GetNumOfContainers() const { return
      Containers()->GetEntriesFast(); }
  TObject      *Container(Int_t ic) const { return
      Containers()->UncheckedAt(ic); }

  virtual void   Browse(TBrowser *b);
  virtual Bool_t IsFolder() const { return kTRUE; }
  virtual void   Clear(Option_t *option = "") { fContainer->Clear(option); }

  virtual void   Init();
  virtual Bool_t ReadSQL(TSQLServer *ser) { return kTRUE; }
  virtual void   AnalyzeBegin();
  virtual void   AnalyzeEntry() = 0;
  virtual void   AnalyzeTerminate();
  virtual void   CreateBranch(TTree *tree);

  ClassDef(TStrelaBase, 1) // Base(virtual) class for STRELA
};

#endif
