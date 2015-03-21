// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    21 Mar 2015

#include "TEventTqdcQ.h"
#include "THitTqdcQ.h"
#include "TVME.h"

ClassImp(TEventTqdcQ)

//______________________________________________________________________________
TEventTqdcQ::TEventTqdcQ()
: TVirtualEvent(),
  fLastHit(0),
  fTrigTs(-1)
{
  // Default constructor
  fHits = new TClonesArray(THitTqdcQ::Class(), 400);

  if (!gVME) return;
  fIdxHitChanLast.Set(gVME->GetNChannelsTQDC());
  for (Int_t multi = 0; multi < kMaxMulti; multi++)
    fIdxHitChanMulti[multi].Set(fIdxHitChanLast.GetSize());
}
//______________________________________________________________________________
TEventTqdcQ::~TEventTqdcQ()
{
  //  Info("~TEventTqdcQ", "Destructor");
}
//______________________________________________________________________________
void TEventTqdcQ::Clear(Option_t *option)
{
  TVirtualEvent::Clear(option);
}
//______________________________________________________________________________
void TEventTqdcQ::Print(Option_t *option) const
{
  THitTqdcQ *hit;
  Printf("next event");
  for (Int_t i = 0; i < fNHits; i++) {
    hit = GetHitTqdcQ(i);
    Printf(" %2d) channel = %3d, Q = %6d, timestamp = %4d, ns = %d",
           i, hit->GetChannel(), hit->GetQ(), hit->GetTimeStamp(), hit->GetNSamples());

    if (!strcmp(option, "nosample")) continue;
    for (Int_t j = 0; j < hit->GetNSamples(); j++) {
      printf("\t  [%02d] sample = %6d  ", j, hit->GetSample()[j]);
      for (Int_t k = 0; k < ((Int_t)(hit->GetSample()[j]/512)); k++) printf(" ");
      printf("*\n");
    }
  }
}
//______________________________________________________________________________
void TEventTqdcQ::AddHitTqdcQ(Int_t ch, Int_t ts, Bool_t adc)
{
  // ADC timestamp, assume first TRIG ts and second ADC ts

  // TRIG ts
  if (!adc) {
    fTrigTs = ts;
    return;
  }

  // ADC ts, new hit
  fLastHit = (THitTqdcQ *)fHits->ConstructedAt(fNHits++);
  if (fTrigTs == -1) {
    Warning("AddHitTqdcQ", "no previous TRIG ts");
    fLastHit->Set(ch, kNoneValue);
  } else
    fLastHit->Set(ch, ts - fTrigTs); // timestamp (ADC ts - TRIG ts)

  fTrigTs = -1;
}
//______________________________________________________________________________
void TEventTqdcQ::NextSampleHit(Int_t ch, Int_t sample) const
{
  // ADC sampling

  if (fLastHit->GetChannel() != ch) {
    Error("NextSampleHit", "unfair channel %d", ch);
  } else
    fLastHit->NextSample(sample);
}
//______________________________________________________________________________
Int_t TEventTqdcQ::Q(Int_t ch, Int_t multi)
{
  Int_t idx = GetIndexHit(ch, multi);
  if (idx == -1) return kNoneValue;

  THitTqdcQ *hit = GetHitTqdcQ(idx);
  if (hit->HasOverflowSample()) return kNoneValue;

  Int_t bl = hit->GetNSamples()*hit->GetSample()[0];
  return (hit->GetQ() - bl);
}
//______________________________________________________________________________
Int_t TEventTqdcQ::TimeStamp(Int_t ch, Int_t multi)
{
  Int_t idx = GetIndexHit(ch, multi);
  if (idx == -1) return kNoneValue;

  THitTqdcQ *hit = GetHitTqdcQ(idx);
  if (hit->HasOverflowSample()) return kNoneValue;

  return hit->GetTimeStamp();
}
//______________________________________________________________________________
Int_t TEventTqdcQ::Sample(Int_t ch, Int_t multi, Int_t samp)
{
  Int_t idx = GetIndexHit(ch, multi);
  if (idx == -1) return kNoneValue;

  THitTqdcQ *hit = GetHitTqdcQ(idx);
  if (hit->HasOverflowSample()) return kNoneValue;

  return hit->GetSample()[samp];
}
