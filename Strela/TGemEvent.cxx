// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    03 Dec 2013

#include "TGemEvent.h"

ClassImp(TAdcHit1)

//______________________________________________________________________________
TAdcHit1::TAdcHit1(UShort_t nadc, UShort_t adc, UShort_t delta) : TAdcHit(nadc, adc)
{
  fDelta = delta;
}
//______________________________________________________________________________

ClassImp(TGemEvent)

//______________________________________________________________________________
TGemEvent::TGemEvent()
{
  //  Info("TGemEvent", "Default constructor");
  fNevent  = 0;
  fNhits   = 0;
  fAdcHits = new TClonesArray("TAdcHit1", 4000);
}
//______________________________________________________________________________
TGemEvent::~TGemEvent()
{
  Info("~TGemEvent", "Destructor");
  if (fAdcHits) fAdcHits->Delete();
  delete fAdcHits;
}
//______________________________________________________________________________
void TGemEvent::AddHit(UShort_t nadc, UShort_t adc)
{
  // need for Monte Carlo
  // generated are only Nadc and Adc, i.e. TAdcHit
  // Delta is unnecessary, i.e. not TAdcHit1

  TClonesArray &hits = *fAdcHits;
  new(hits[fNhits++]) TAdcHit(nadc, adc);
}
//______________________________________________________________________________
void TGemEvent::AddHit1(UShort_t nadc, UShort_t adc, UShort_t delta)
{
  TClonesArray &hits1 = *fAdcHits;
  new(hits1[fNhits++]) TAdcHit1(nadc, adc, delta);
}
//______________________________________________________________________________
void TGemEvent::Clear(Option_t *option)
{
  fNhits = 0;
  fAdcHits->Clear(option);
}
//______________________________________________________________________________
void TGemEvent::Print(Option_t * /*option*/) const
{
  Printf("event = %d, nhits = %d", GetNEvent(), GetNHits());
  TAdcHit1 *hit1;

  for (Int_t ih = 0; ih < GetNumOfAdcHits1(); ih++) {
    hit1 = GetAdcHit1(ih);
    Printf(" %2d) nadc = %5d, adc = %5d, delta = %3d",
           ih, hit1->GetNadc(), hit1->GetAdc(), hit1->GetDelta());
  }
}
