// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 24 Oct 2007

#ifndef STRELA_TStrawEvent
#define STRELA_TStrawEvent

#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif

class TStrawHit : public TObject {

private:
  UShort_t      fNtdc;  //  Ntdc
  UShort_t      fTdc;   //  Tdc
  UShort_t      fDelta; //  Delta

public:
  TStrawHit() {;}
  virtual      ~TStrawHit() {;}

  Int_t         GetNtdc() const { return (Int_t)fNtdc; }
  Int_t         GetTdc() const { return (Int_t)fTdc; }
  Int_t         GetDelta() const { return (Int_t)fDelta; }

  ClassDef(TStrawHit, 2) // StrawHit class (not original)
};

//______________________________________________________________________________

class TStrawEvent : public TObject {

private:
  Int_t         fNevent;  //  Nevent
  Int_t         fNstraws; //  Nstraws
  TClonesArray *fStraws;  //->array with straw hits

public:
  TStrawEvent();
  virtual      ~TStrawEvent();

  Int_t         GetNEvent() const { return fNevent; }
  Int_t         GetNStraws() const { return fNstraws; }
  TClonesArray *StrawHits() const { return fStraws; }
  Int_t         GetNumOfStraws() const { return fStraws->GetEntriesFast(); }
  TStrawHit    *GetStrawHit(Int_t is) const { return
      (TStrawHit *)fStraws->UncheckedAt(is); }

  ClassDef(TStrawEvent, 2) // StrawEvent class (not original)
};

#endif
