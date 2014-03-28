// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    28 Mar 2014

#include "TVMEEvent.h"

ClassImp(TTDCHit)

//______________________________________________________________________________
TTDCHit::TTDCHit(Int_t ch, Int_t tld)
: TObject(),
  fChannel(ch),
  fTime(tld),
  fDelta(0)
{
  // Normal constructor
}
//______________________________________________________________________________
void TTDCHit::AddDelta(Int_t ttr)
{
  if (ttr < fTime) {
    Warning("AddDelta", "TLD > TTR (%6d > %6d)", fTime, ttr);
    return;
  }

  fDelta = ttr - fTime; // trailing - leading
}

//______________________________________________________________________________

ClassImp(TVMEEvent)

//______________________________________________________________________________
TVMEEvent::TVMEEvent()
: TObject(),
  fEvent(0),
  fNTDCHits(0),
  fTDCHits(0),
  fPosTDCHitChan(0)
{
  // Default constructor
  fTDCHits = new TClonesArray("TTDCHit", 1000);
  fPosTDCHitChan = new Int_t[999]; // gVME->GetNChannels();
}
//______________________________________________________________________________
TVMEEvent::~TVMEEvent()
{
  Info("~TVMEEvent", "Destructor");
  if (fTDCHits) fTDCHits->Delete();
  SafeDelete(fTDCHits);
  delete [] fPosTDCHitChan;
}
//______________________________________________________________________________
void TVMEEvent::Clear(Option_t *option)
{
  fNTDCHits = 0;
  fTDCHits->Clear(option);
  memset(fPosTDCHitChan, 0, sizeof(Int_t)*999); // gVME->GetNChannels();
}
//______________________________________________________________________________
void TVMEEvent::Print(Option_t * /*option*/) const
{
  TTDCHit *hit;
  for (Int_t i = 0; i < GetNumOfTDCHits(); i++) {
    hit = GetTDCHit(i);
    Printf(" %2d) time = %6d, delta = %6d", i, hit->GetTime(), hit->GetDelta());
  }
}
//______________________________________________________________________________
void TVMEEvent::AddTDCHit(Int_t ch, Int_t tdc, Bool_t lead)
{
  // fast variant pre only leading

  // 1) preverovat je dalsi TLD vacsi ako predchadzajuci ?
  // 2) preverovat je dalsi TRL vacsi ako predchdzajuci TRL a zaroven ako TLD
  // potial OK

  // 3) pre kazdy kanal extra !!! velmi neefektivne !!! bude lepsie raz cez vsetky kanaly
  // pre 20 hit bude 210 cyklov, pre 30 hitov bude 465 cyklov
  // vytvorit mapu s hitami !!!

  TClonesArray &hits = *fTDCHits;
  new(hits[fNTDCHits++]) TTDCHit(ch, tdc);
}
