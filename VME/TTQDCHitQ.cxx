// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    16 Mar 2015

#include "TTQDCHitQ.h"

ClassImp(TTQDCHitQ)

//______________________________________________________________________________
TTQDCHitQ::TTQDCHitQ()
: TObject(),
  fChannelQ(0),
  fQ(0),
  fTimeStamp(0),
  fNSamples(0),
  fSample(0),
  fMaxSamples(8)
{
  //  Info("TTQDCHitQ", "Constructor");

  // use only for event with TClonesArray::ConstructedAt
  fSample = new Short_t[fMaxSamples];
}
//______________________________________________________________________________
TTQDCHitQ::~TTQDCHitQ()
{
  //  Info("~TTQDCHitQ", "Destructor");
  if (fSample) delete [] fSample;
}
//______________________________________________________________________________
void TTQDCHitQ::Set(Int_t ch, Int_t ts)
{
  fChannelQ  = ch;
  fQ         = 0;
  fTimeStamp = ts;
  fNSamples  = 0;
  memset(fSample, 0, fMaxSamples*sizeof(Short_t));
}
//______________________________________________________________________________
void TTQDCHitQ::NextSample(Int_t sample)
{
  fQ += sample;

  if (fNSamples >= fMaxSamples) ExpandSamples(fNSamples + 1);
  fSample[fNSamples++] = sample;
}
//______________________________________________________________________________
void TTQDCHitQ::ExpandSamples(Int_t nsize)
{
  //  Info("ExpandSamples", "%d => %d", fMaxSamples, nsize);

  Short_t *narray = new Short_t[nsize];
  memset(narray, 0, nsize*sizeof(Short_t));
  for (Int_t i = 0; i < fMaxSamples; i++) narray[i] = fSample[i];

  delete [] fSample;
  fSample = narray;
  fMaxSamples = nsize;
}
