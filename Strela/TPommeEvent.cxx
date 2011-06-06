// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    26 May 2011

#include "TPommeEvent.h"

ClassImp(TChWire)

//______________________________________________________________________________

ClassImp(TAdcHit)

//______________________________________________________________________________
TAdcHit::TAdcHit(UShort_t nadc, UShort_t adc)
{
  fNadc = nadc;
  fAdc  = adc;
}
//______________________________________________________________________________

ClassImp(TPommeEvent)

//______________________________________________________________________________
TPommeEvent::TPommeEvent()
{
  //  Info("TPommeEvent", "Default Constructor");
  fNevent  = 0;
  fNwires  = 0;
  fWires   = new TClonesArray("TChWire", 50);
  fNhits   = 0;
  fAdcHits = new TClonesArray("TAdcHit", 50);
}
//______________________________________________________________________________
TPommeEvent::~TPommeEvent()
{
  Info("~TPommeEvent", "Destructor");
  if (fWires) fWires->Delete();
  delete fWires;
  if (fAdcHits) fAdcHits->Delete();
  delete fAdcHits;
}
