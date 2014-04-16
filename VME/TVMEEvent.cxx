// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    16 Apr 2014

#include "TVMEEvent.h"
#include "TVME.h"

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
void TTDCHit::SetDelta(Int_t ttr)
{
  if (fTime > ttr) Warning("SetDelta", "TLD > TTR");
  else fDelta = ttr - fTime; // trailing - leading
}
//______________________________________________________________________________

Int_t TVMEEvent::fgTrigChannel = -1;

ClassImp(TVMEEvent)

//______________________________________________________________________________
TVMEEvent::TVMEEvent()
: TObject(),
  fEvent(0),
  fNTDCHits(0),
  fTDCHits(0),
  fIdxTDCHitChan()
{
  // Default constructor
  fTDCHits = new TClonesArray(TTDCHit::Class(), 2000);
  if (!gVME) return;

  fIdxTDCHitChan.Set(gVME->GetNChannelsFast());
  for (Int_t i = 0; i < kMaxMulti; i++)
    fIdxTDCHitChanMulti[i].Set(fIdxTDCHitChan.GetSize());
}
//______________________________________________________________________________
TVMEEvent::~TVMEEvent()
{
  Info("~TVMEEvent", "Destructor");
  if (fTDCHits) fTDCHits->Delete();
  SafeDelete(fTDCHits);
}
//______________________________________________________________________________
void TVMEEvent::Clear(Option_t *option)
{
  fNTDCHits = 0;
  fTDCHits->Clear(option);
  fIdxTDCHitChan.Reset(); // also OK with no (zero) elements
}
//______________________________________________________________________________
void TVMEEvent::Print(Option_t * /*option*/) const
{
  TTDCHit *hit;
  for (Int_t i = 0; i < GetNumOfTDCHits(); i++) {
    hit = GetTDCHit(i);
    Printf(" %2d) channel = %3d, time = %6d, delta = %4d",
           i, hit->GetChannel(), hit->GetTime(), hit->GetDelta());
  }
}
//______________________________________________________________________________
void TVMEEvent::AddTDCHit(Int_t ch, Int_t tld)
{
  // leading tdc

  TClonesArray &hits = *fTDCHits;
  new(hits[fNTDCHits++]) TTDCHit(ch, tld);

  if (fIdxTDCHitChan.GetSize() > 0) fIdxTDCHitChan.AddAt(fNTDCHits, ch);
}
//______________________________________________________________________________
void TVMEEvent::AddTDCHitCheck(Int_t ch, Int_t tdc, Bool_t ld)
{
  // leading or trailing tdc, assume sorted tdc time

  Int_t prevIdx = fIdxTDCHitChan.At(ch);
  if (prevIdx == 0) { // no previous hit of channel
    if (ld) AddTDCHit(ch, tdc);
    return;
  }

  TTDCHit *prevHit = GetTDCHit(prevIdx-1);
  if (!prevHit) {
    Error("AddTDCHitCheck", "no previous hit at %d", prevIdx-1);
    return;
  }
  if (prevHit->GetChannel() != ch) {
    Error("AddTDCHitCheck", "unfair channel %d", ch);
    return;
  }

  // leading after leading
  if (ld) {
    if (prevHit->GetTLD() > tdc)
      Warning("AddTDCHitCheck", "time (TLD) is not sorting");
    AddTDCHit(ch, tdc);
    return;
  }

  // trailing after leading (if) or trailing (else)
  if (prevHit->GetDelta() == 0) prevHit->SetDelta(tdc); // and check on TTR > TLD
  else if (prevHit->GetTTR() > tdc) {
    Warning("AddTDCHitCheck", "time (TTR) is not sorting");
    prevHit->SetDelta(tdc);
  }
}



//______________________________________________________________________________
void TVMEEvent::SetTrigChannel(Int_t ch)
{
  // static function
  fgTrigChannel = ch;
}
//______________________________________________________________________________
