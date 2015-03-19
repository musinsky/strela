// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Mar 2015

#include "TEventTdc.h"
#include "THitTdc.h"
#include "TVME.h"

Int_t TEventTdc::fgTrigChannel = -1;
Int_t TEventTdc::fgTrigOffset  = 0;

ClassImp(TEventTdc)

//______________________________________________________________________________
TEventTdc::TEventTdc()
: TVirtualEvent(),
  fTrigTime(0)
{
  // Default constructor
  fHits = new TClonesArray(THitTdc::Class(), 2000);

  if (!gVME) return;
  fIdxHitChanLast.Set(gVME->GetNChannelsTDC());
  for (Int_t multi = 0; multi < kMaxMulti; multi++)
    fIdxHitChanMulti[multi].Set(fIdxHitChanLast.GetSize());
}
//______________________________________________________________________________
TEventTdc::~TEventTdc()
{
  //  Info("~TEventTdc", "Destructor");
}
//______________________________________________________________________________
void TEventTdc::Clear(Option_t *option)
{
  TVirtualEvent::Clear(option);
  fIdxHitChanLast.Reset(); // also OK with no elements (empty array)
  fTrigTime = 0;           // must be 0 (unless trigger)
}
//______________________________________________________________________________
void TEventTdc::Print(Option_t * /*option*/) const
{
  THitTdc *hit;
  Printf("next event");
  for (Int_t i = 0; i < fNHits; i++) {
    hit = GetHitTdc(i);
    Printf(" %2d) channel = %3d, time = %6d, delta = %4d",
           i, hit->GetChannel(), hit->GetTime(), hit->GetDelta());
  }
}
//______________________________________________________________________________
void TEventTdc::AddHitTdc(Int_t ch, Int_t tld)
{
  // leading tdc

  THitTdc *hit = (THitTdc *)fHits->ConstructedAt(fNHits++);
  hit->Set(ch, tld);

  if (fIdxHitChanLast.GetSize() > 0) fIdxHitChanLast.AddAt(fNHits, ch);

  // trigger time, if multi then first hit time, i.e. same as Time(fgTrigChannel, 0)
  if ((ch == fgTrigChannel) && (fTrigTime == 0)) fTrigTime = tld;
}
//______________________________________________________________________________
void TEventTdc::AddHitTdcCheck(Int_t ch, Int_t tdc, Bool_t ld)
{
  // leading or trailing tdc, assume sorted tdc time

  Int_t prevIdx = fIdxHitChanLast.At(ch)-1;
  if (prevIdx == -1) { // no previous hit of channel
    if (ld) AddHitTdc(ch, tdc);
    return;
  }

  THitTdc *prevHit = GetHitTdc(prevIdx);
  if (!prevHit) {
    Error("AddHitTdcCheck", "no previous hit at %d", prevIdx);
    return;
  }
  if (prevHit->GetChannel() != ch) {
    Error("AddHitTdcCheck", "unfair channel %d", ch);
    return;
  }

  // leading after leading
  if (ld) {
    if (prevHit->GetTLD() > tdc)
      Warning("AddHitTdcCheck", "time (TLD) is not sorting");
    AddHitTdc(ch, tdc);
    return;
  }

  // trailing after leading (if) or trailing (else)
  if (prevHit->GetDelta() == 0) prevHit->SetDelta(tdc); // and check on TTR > TLD
  else if (prevHit->GetTTR() > tdc) {
    Warning("AddHitTdcCheck", "time (TTR) is not sorting");
    prevHit->SetDelta(tdc);
  }
}
//______________________________________________________________________________
void TEventTdc::SetTrigInfo(Int_t channel, Int_t offset)
{
  // static function

  if (channel < 0) { channel = -1; offset = 0; } // reset
  fgTrigChannel = channel;
  fgTrigOffset  = offset;
}
//______________________________________________________________________________
Int_t TEventTdc::Time(Int_t ch, Int_t multi)
{
  Int_t idx = GetIndexHit(ch, multi);
  if (idx == -1) return kNoneValue;

  Int_t time = GetHitTdc(idx)->GetTime();
  if ((fgTrigChannel < 0) || (ch == fgTrigChannel)) return time;

  //  if (Multi(fgTrigChannel) != 1) {
  //    Warning("Time", "not exactly one trigger channel per event");
  //    //    return kNoneValue;
  //  }

  //  return (time - GetHitTdc(GetIndexHit(fgTrigChannel, 0))->GetTime() + fgTrigOffset);
  return (time - fTrigTime + fgTrigOffset);
}
//______________________________________________________________________________
Int_t TEventTdc::Delta(Int_t ch, Int_t multi)
{
  Int_t idx = GetIndexHit(ch, multi);
  if (idx == -1) return kNoneValue;

  return GetHitTdc(idx)->GetDelta();
}
