// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Apr 2014

#include "TVMEEvent.h"
#include "TTDCHit.h"
#include "TVME.h"

const Int_t kNoneValue = -9999;

Int_t TVMEEvent::fgTrigChannel = -1;
Int_t TVMEEvent::fgTrigOffset  = 0;

ClassImp(TVMEEvent)

//______________________________________________________________________________
TVMEEvent::TVMEEvent()
: TObject(),
  fEvent(0),
  fNTDCHits(0),
  fTDCHits(0),
  fIdxTDCHitChanLast()
{
  // Default constructor
  fTDCHits = new TClonesArray(TTDCHit::Class(), 2000);
  if (!gVME) return;

  // or implement function for manual set size of array(s)
  fIdxTDCHitChanLast.Set(gVME->GetNChannelsFast());
  for (Int_t multi = 0; multi < kMaxMulti; multi++)
    fIdxTDCHitChanMulti[multi].Set(fIdxTDCHitChanLast.GetSize());
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
  fIdxTDCHitChanLast.Reset(); // also OK with no (zero) elements
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

  if (fIdxTDCHitChanLast.GetSize() > 0) fIdxTDCHitChanLast.AddAt(fNTDCHits, ch);
}
//______________________________________________________________________________
void TVMEEvent::AddTDCHitCheck(Int_t ch, Int_t tdc, Bool_t ld)
{
  // leading or trailing tdc, assume sorted tdc time

  Int_t prevIdx = fIdxTDCHitChanLast.At(ch)-1;
  if (prevIdx == -1) { // no previous hit of channel
    if (ld) AddTDCHit(ch, tdc);
    return;
  }

  TTDCHit *prevHit = GetTDCHit(prevIdx);
  if (!prevHit) {
    Error("AddTDCHitCheck", "no previous hit at %d", prevIdx);
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
void TVMEEvent::SetTrigInfo(Int_t channel, Int_t offset)
{
  // static function
  fgTrigChannel = channel;
  fgTrigOffset  = offset;
}
//______________________________________________________________________________
void TVMEEvent::IndexTDCHitChanMulti()
{
  // indexing all hits, must be call (but only once) for each given event

  fIdxTDCHitChanLast.Reset();
  for (Int_t multi = 0; multi < kMaxMulti; multi++)
    fIdxTDCHitChanMulti[multi].Reset();

  for (Int_t i = 0; i < fNTDCHits; i++) {
    Int_t ch = GetTDCHit(i)->GetChannel();
    for (Int_t multi = 0; multi < kMaxMulti; multi++)
      if (fIdxTDCHitChanMulti[multi].At(ch) == 0) {
        fIdxTDCHitChanMulti[multi].AddAt(i+1, ch);
        break;
      }
    fIdxTDCHitChanLast.AddAt(i+1, ch); // always last hit
  }

  SetBit(kNextEvent, kTRUE); // only once
}
//______________________________________________________________________________
Int_t TVMEEvent::GetIndexTDCHit(Int_t ch, Int_t multi)
{
  // trick how call (only once) IndexTDCHitChanMulti() for each given event
  // bit kNextEvent from stored event (from root file) is always kFALSE
  // works wih TTree::Draw (Scan or Query even thought call more than once)
  if (!TestBit(kNextEvent)) IndexTDCHitChanMulti();

  if ((multi < 0) || (multi >= kMaxMulti))
    return (fIdxTDCHitChanLast.At(ch)-1);
  else
    return (fIdxTDCHitChanMulti[multi].At(ch)-1);
}
//______________________________________________________________________________
Int_t TVMEEvent::Multi(Int_t ch)
{
  Int_t lastIdx = GetIndexTDCHit(ch, -1);
  if (lastIdx == -1) return 0;

  for (Int_t multi = 0; multi < kMaxMulti; multi++)
    if ((fIdxTDCHitChanMulti[multi].At(ch)-1) == lastIdx) return (multi+1);

  return (kMaxMulti+1);
}
//______________________________________________________________________________
Int_t TVMEEvent::Time(Int_t ch, Int_t multi)
{
  Int_t idx = GetIndexTDCHit(ch, multi);
  if (idx == -1) return kNoneValue;

  Int_t time = GetTDCHit(idx)->GetTime();
  if ((fgTrigChannel < 0) || (ch == fgTrigChannel)) return time;

  if (Multi(fgTrigChannel) != 1) {
    Warning("Time", "not exactly one trigger channel per event");
    return kNoneValue;
  }

  return (time - GetTDCHit(GetIndexTDCHit(fgTrigChannel, -1))->GetTime() + fgTrigOffset);
}
//______________________________________________________________________________
Int_t TVMEEvent::Delta(Int_t ch, Int_t multi)
{
  Int_t idx = GetIndexTDCHit(ch, multi);
  if (idx == -1) return kNoneValue;

  return GetTDCHit(idx)->GetDelta();
}
