// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    26 May 2011

#ifndef STRELA_TGemEvent
#define STRELA_TGemEvent

#ifndef STRELA_TPommeEvent
#include "TPommeEvent.h"
#endif

class TAdcHit1 : public TAdcHit {

private:
  UShort_t      fDelta; //  Delta

public:
  TAdcHit1() {;}
  virtual      ~TAdcHit1() {;}

  Int_t         GetDelta() const { return (Int_t)fDelta; }

  ClassDef(TAdcHit1, 2) // AdcHit1 class (not original)
};

//______________________________________________________________________________

class TGemEvent : public TObject {

private:
  UInt_t        fNevent;  //  Nevent
  UInt_t        fNhits;   //  Nhits
  TClonesArray *fAdcHits; //->array with adc hits

public:
  TGemEvent();
  virtual      ~TGemEvent();

  Int_t         GetNEvent() const { return (Int_t)fNevent; }
  Int_t         GetNHits() const { return (Int_t)fNhits; }
  TClonesArray *AdcHits1() const { return fAdcHits; }
  Int_t         GetNumOfAdcHits1() const { return fAdcHits->GetEntriesFast(); }
  TAdcHit1     *GetAdcHit1(Int_t ia) const { return (TAdcHit1 *)fAdcHits->UncheckedAt(ia); }
  void          AddHit(UShort_t nadc, UShort_t adc);

  virtual void  Clear(Option_t *option = "");
  virtual void  Print(Option_t *option = "") const;

  ClassDef(TGemEvent, 3) // GemEvent class (not original)
};

#endif
