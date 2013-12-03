// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    04 Dec 2013

#ifndef STRELA_TMultiHit
#define STRELA_TMultiHit

#include <TClonesArray.h>

class TMHit : public TObject {

private:
  Int_t          fTime; // time
  Bool_t         fLead; // leading OR trailing

public:
  TMHit() : fTime(0), fLead(kFALSE) {;}
  TMHit(Int_t time, Bool_t lead);
  virtual ~TMHit() {;}

  Int_t          GetTime() const { return fTime; }
  Bool_t         GetLead() const { return fLead; }
  virtual Int_t  Compare(const TObject *obj) const;
  virtual Bool_t IsSortable() const { return kTRUE; }

  ClassDef(TMHit, 1) // MHit class
};

//______________________________________________________________________________

class TMultiHit : public TObject {

private:
  Int_t          fNHits; // NHits
  TClonesArray  *fHits;  //->array with mhits

public:
  TMultiHit();
  virtual ~TMultiHit();

  Int_t          GetNHits() const { return fNHits; }
  TClonesArray  *Hits() const { return fHits; }
  Int_t          GetNumOfHits() const { return fHits->GetEntriesFast(); }
  TMHit         *GetHit(Int_t ih) const { return (TMHit *)fHits->UncheckedAt(ih); }
  void           AddHit(Int_t time, Bool_t lead);
  virtual void   Clear(Option_t *option = "");
  virtual void   Print(Option_t *option = "") const;

  ClassDef(TMultiHit, 1) // MultiHit class
};

#endif
