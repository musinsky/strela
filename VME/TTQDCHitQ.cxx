// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    10 Mar 2015

#include "TTQDCHitQ.h"

ClassImp(TTQDCHitQ)

//______________________________________________________________________________
TTQDCHitQ::TTQDCHitQ()
: TObject(),
  fChannelQ(0),
  fQ(0),
  fTimeStamp(0),
  fNSamples(0)
{
  //  Info("TTQDCHitQ", "Constructor");
}
//______________________________________________________________________________
TTQDCHitQ::~TTQDCHitQ()
{
  //  Info("~TTQDCHitQ", "Destructor");
}
//______________________________________________________________________________
void TTQDCHitQ::Set(Int_t ch, Int_t ts)
{
  fChannelQ  = ch;
  fQ         = 0;
  fTimeStamp = ts;
  fNSamples  = 0;
  memset(fSample, 0, sizeof(fSample));
}
//______________________________________________________________________________
void TTQDCHitQ::NextSample(Int_t sample)
{
  fQ += sample;

  if (fNSamples >= kMaxSamples)
    Warning("NextSample", "to many samples, limit is %d", kMaxSamples);
  else
    fSample[fNSamples++] = sample;
}
