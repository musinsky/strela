// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    26 May 2011

#include "TGemEvent.h"

ClassImp(TAdcHit1)

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
void TGemEvent::Clear(Option_t *option)
{
  fNhits = 0;
  fAdcHits->Clear(option);
}
//______________________________________________________________________________
void TGemEvent::Print(Option_t *option) const
{
  Printf("event = %d, nhits = %d", GetNEvent(), GetNHits());
  TAdcHit1 *hit1;

  for (Int_t ih = 0; ih < GetNumOfAdcHits1(); ih++) {
    hit1 = GetAdcHit1(ih);
    Printf(" %2d) nadc = %5d, adc = %5d, delta = %d",
           ih, hit1->GetNadc(), hit1->GetAdc(), hit1->GetDelta());
  }
}
