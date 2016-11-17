// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    17 Nov 2016

#include "TEventTqdcT.h"
#include "THitTqdcT.h"
#include "TVME.h"
#include "TEventTdc.h"

ClassImp(TEventTqdcT)

//______________________________________________________________________________
TEventTqdcT::TEventTqdcT()
: TVirtualEvent(),
  fTrigTime(0)
{
  // Default constructor
  fHits = new TClonesArray(THitTqdcT::Class(), 500);

  if (!gVME) return;
  fIdxHitChanLast.Set(gVME->GetNChannelsTQDC());
  for (Int_t multi = 0; multi < kMaxMulti; multi++)
    fIdxHitChanMulti[multi].Set(fIdxHitChanLast.GetSize());
}
//______________________________________________________________________________
TEventTqdcT::~TEventTqdcT()
{
  //  Info("~TEventTqdcT", "Destructor");
}
//______________________________________________________________________________
void TEventTqdcT::Clear(Option_t *option)
{
  TVirtualEvent::Clear(option);
  fIdxHitChanLast.Reset(); // also OK with no elements (empty array)
  fTrigTime = 0;           // must be 0 (unless trigger)
}
//______________________________________________________________________________
void TEventTqdcT::Print(Option_t * /*option*/) const
{
  THitTqdcT *hit;
  Printf("next event");
  for (Int_t i = 0; i < fNHits; i++) {
    hit = GetHitTqdcT(i);
    Printf(" %2d) channel = %3d, time = %6d, delta = %4d",
           i, hit->GetChannel(), hit->GetTime(), hit->GetDelta());
  }
}
//______________________________________________________________________________
void TEventTqdcT::AddHitTqdcT(Int_t ch, Int_t tld)
{
  // leading tdc

  THitTqdcT *hit = (THitTqdcT *)fHits->ConstructedAt(fNHits++);
  hit->Set(ch, tld);

  if (fIdxHitChanLast.GetSize() > 0) fIdxHitChanLast.AddAt(fNHits, ch);
}
//______________________________________________________________________________
void TEventTqdcT::AddHitTqdcTCheck(Int_t ch, Int_t tdc, Bool_t ld)
{
  // leading or trailing tdc, assume sorted tdc time

  Int_t prevIdx = fIdxHitChanLast.At(ch)-1;
  if (prevIdx == -1) { // no previous hit of channel
    if (ld) AddHitTqdcT(ch, tdc);
    return;
  }

  THitTqdcT *prevHit = GetHitTqdcT(prevIdx);
  if (!prevHit) {
    Error("AddHitTqdcTCheck", "no previous hit at %d", prevIdx);
    return;
  }
  if (prevHit->GetChannel() != ch) {
    Error("AddHitTqdcTCheck", "unfair channel %d", ch);
    return;
  }

  // leading after leading
  if (ld) {
    if (prevHit->GetTLD() > tdc)
      Warning("AddHitTqdcTCheck", "time (TLD) is not sorting");
    AddHitTqdcT(ch, tdc);
    return;
  }

  // trailing after leading (if) or trailing (else)
  if (prevHit->GetDelta() == 0) prevHit->SetDelta(tdc); // and check on TTR > TLD
  else if (prevHit->GetTTR() > tdc) {
    Warning("AddHitTqdcTCheck", "time (TTR) is not sorting");
    prevHit->SetDelta(tdc);
  }
}
//______________________________________________________________________________
Int_t TEventTqdcT::Time(Int_t ch, Int_t multi)
{
  Int_t idx = GetIndexHit(ch, multi);
  if (idx == -1) return kNoneValue;

  Int_t time = GetHitTqdcT(idx)->GetTime();
  if (TEventTdc::GetTrigChannel() < 0) return time;
  if (fTrigTime == 0) return time; // fTrigTime (for TQDC) is set until DecodeETRL (from TDC)
  return (time - fTrigTime + TEventTdc::GetTrigOffset());
}
//______________________________________________________________________________
Int_t TEventTqdcT::Delta(Int_t ch, Int_t multi)
{
  Int_t idx = GetIndexHit(ch, multi);
  if (idx == -1) return kNoneValue;

  return GetHitTqdcT(idx)->GetDelta();
}
