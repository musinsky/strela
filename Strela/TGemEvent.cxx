// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 25 Oct 2007

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
void TGemEvent::Print(Option_t *option) const
{
  Printf("evnet = %d, nhits = %d", GetNEvent(), GetNHits());
  TAdcHit1 *hit1;

  for (Int_t ih = 0; ih < GetNumOfAdcHits1(); ih++) {
    hit1 = GetAdcHit1(ih);
    Printf(" %2d) nadc = %5d, adc = %5d, delta = %d",
           ih, hit1->GetNadc(), hit1->GetAdc(), hit1->GetDelta());
  }
}
