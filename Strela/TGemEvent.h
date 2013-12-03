// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    04 Dec 2013

#ifndef STRELA_TGemEvent
#define STRELA_TGemEvent

#ifndef STRELA_TPommeEvent
#include "TPommeEvent.h"
#endif

class TAdcHit1 : public TAdcHit {

private:
  UShort_t      fDelta; //  Delta

public:
  TAdcHit1() : fDelta(0) {;}
  TAdcHit1(UShort_t nadc, UShort_t adc, UShort_t delta);
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

  void          SetEvent(Int_t e) { fNevent = e; }
  Int_t         GetNEvent() const { return (Int_t)fNevent; }
  Int_t         GetNHits() const { return (Int_t)fNhits; }
  TClonesArray *AdcHits1() const { return fAdcHits; }
  Int_t         GetNumOfAdcHits1() const { return fAdcHits->GetEntriesFast(); }
  TAdcHit1     *GetAdcHit1(Int_t ia) const { return (TAdcHit1 *)fAdcHits->UncheckedAt(ia); }
  void          AddHit(UShort_t nadc, UShort_t adc);
  void          AddHit1(UShort_t nadc, UShort_t adc, UShort_t delta);

  virtual void  Clear(Option_t *option = "");
  virtual void  Print(Option_t *option = "") const;

  ClassDef(TGemEvent, 3) // GemEvent class (not original)
};

#endif
