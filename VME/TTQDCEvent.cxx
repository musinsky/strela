// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    10 Mar 2015

#include "TTQDCEvent.h"
#include "TTQDCHitT.h"
#include "TTQDCHitQ.h"

ClassImp(TTQDCEvent)

//______________________________________________________________________________
TTQDCEvent::TTQDCEvent()
: TObject(),
  fEvent(0),
  fNHitsT(0),
  fHitsT(0),
  fNHitsQ(0),
  fHitsQ(0),
  fHitQLast(0),
  fHitQTimeStamp(-1)
{
  // Default constructor
  fHitsT = new TClonesArray(TTQDCHitT::Class(), 200);
  fHitsQ = new TClonesArray(TTQDCHitQ::Class(), 100);
}
//______________________________________________________________________________
TTQDCEvent::~TTQDCEvent()
{
  Info("~TTQDCEvent", "Destructor");
  if (fHitsT) fHitsT->Delete();
  SafeDelete(fHitsT);
  if (fHitsQ) fHitsQ->Delete();
  SafeDelete(fHitsQ);
}
//______________________________________________________________________________
void TTQDCEvent::Clear(Option_t *option)
{
  fNHitsT = 0;
  fHitsT->Clear(option);
  fNHitsQ = 0;
  fHitsQ->Clear(option);
}
//______________________________________________________________________________
void TTQDCEvent::Print(Option_t *option) const
{
  TTQDCHitT *hitT;
  TTQDCHitQ *hitQ;

  Printf("event: %d", fEvent);

  for (Int_t i = 0; i < GetNumOfHitsT(); i++) {
    hitT = GetHitT(i);
    Printf(" %2d) channel = %3d, T = %6d",
           i, hitT->GetChannelT(), hitT->GetT());
  }

  for (Int_t i = 0; i < GetNumOfHitsQ(); i++) {
    hitQ = GetHitQ(i);
    Printf(" %2d) channel = %3d, Q = %6d, timestamp = %4d, ns = %d",
           i, hitQ->GetChannelQ(), hitQ->GetQ(), hitQ->GetTimeStamp(), hitQ->GetNSamples());

    if (strcmp(option, "sample")) continue;
    for (Int_t j = 0; j < hitQ->GetNSamples(); j++) {
      printf("\t  [%02d] sample = %6d  ", j, hitQ->GetSample()[j]);
      for (Int_t k = 0; k < ((Int_t)(hitQ->GetSample()[j]/512)); k++) printf(" ");
      printf("*\n");
    }
  }
}
//______________________________________________________________________________
void TTQDCEvent::AddHitT(Int_t ch, Int_t t)
{
  // TDC data

  TTQDCHitT *hit = (TTQDCHitT *)fHitsT->ConstructedAt(fNHitsT++);
  hit->Set(ch, t);
}
//______________________________________________________________________________
void TTQDCEvent::AddHitQ(Int_t ch, Int_t ts, Bool_t adc)
{
  // ADC timestamp, assume first TRIG ts and second ADC ts

  if (adc) {             // ADC ts, new hit
    fHitQLast = (TTQDCHitQ *)fHitsQ->ConstructedAt(fNHitsQ++);
    fHitQLast->Set(ch, ts - fHitQTimeStamp); // timestamp (ADC ts - TRIG ts)

    if (fHitQTimeStamp == -1) Warning("AddHitQ", "no previous TRIG ts");
    fHitQTimeStamp = -1;
  } else
    fHitQTimeStamp = ts; // TRIG ts
}
//______________________________________________________________________________
void TTQDCEvent::NextSampleHitQ(Int_t ch, Int_t sample) const
{
  // ADC sampling

  if (fHitQLast->GetChannelQ() != ch) {
    Warning("NextSampleHitQ", "unfair channel %d", ch);
  } else
    fHitQLast->NextSample(sample);
}
