// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    21 Mar 2015

#include "THitTqdcQ.h"
#include "TVMERawData.h"

ClassImp(THitTqdcQ)

//______________________________________________________________________________
THitTqdcQ::THitTqdcQ()
: TVirtualHit(),
  fQ(0),
  fTimeStamp(0),
  fNSamples(0),
  fSample(0),
  fMaxSamples(8)
{
  // use THitTqdcQ only for event with TClonesArray::ConstructedAt
  fSample = new Short_t[fMaxSamples];
}
//______________________________________________________________________________
THitTqdcQ::~THitTqdcQ()
{
  if (fSample) delete [] fSample;
}
//______________________________________________________________________________
void THitTqdcQ::Set(Int_t ch, Int_t ts)
{
  fChannel   = ch;
  fQ         = 0;
  fTimeStamp = ts;
  fNSamples  = 0;
  memset(fSample, 0, fMaxSamples*sizeof(Short_t));
  ResetBit(kOverflow);
}
//______________________________________________________________________________
void THitTqdcQ::NextSample(Int_t sample)
{
  fQ += sample;
  if (sample >= ((1 << (TVMERawData::kADCBits - 1)) - 1)) SetBit(kOverflow); // 8191

  if (fNSamples >= fMaxSamples) ExpandSamples(fNSamples + 1);
  fSample[fNSamples++] = sample;
}
//______________________________________________________________________________
void THitTqdcQ::ExpandSamples(Int_t nsize)
{
  //  Info("ExpandSamples", "%d => %d", fMaxSamples, nsize);

  Short_t *narray = new Short_t[nsize];
  memset(narray, 0, nsize*sizeof(Short_t));
  for (Int_t i = 0; i < fMaxSamples; i++) narray[i] = fSample[i];

  delete [] fSample;
  fSample = narray;
  fMaxSamples = nsize;
}
