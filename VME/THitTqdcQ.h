// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    21 Mar 2015

#ifndef STRELA_THitTqdcQ
#define STRELA_THitTqdcQ

#include "TVirtualHit.h"

class THitTqdcQ : public TVirtualHit {

public:
  // status bit for overflow sample
  enum { kOverflow = BIT(14) };

  THitTqdcQ();
  virtual ~THitTqdcQ();

  Int_t         GetChannelTqdcQ() const { return GetChannel(); }
  Int_t         GetQ() const { return fQ; }
  Int_t         GetTimeStamp() const { return fTimeStamp; }
  Int_t         GetNSamples() const { return fNSamples; }
  Short_t      *GetSample() { return fSample; }
  Bool_t        HasOverflowSample() const { return TestBit(kOverflow); }
  void          Set(Int_t ch, Int_t ts);
  void          NextSample(Int_t sample);

private:
  Int_t         fQ;         // charge (sum of all samples)
  Short_t       fTimeStamp; // timestamp (ADC ts - TRIG ts)
  Int_t         fNSamples;  // number of samples
  Short_t      *fSample;    //[fNSamples] array of samples (14 bits word, signed)

  Int_t         fMaxSamples; //! maximum number of samples
  void          ExpandSamples(Int_t nsize);

  ClassDef(THitTqdcQ, 1) // HitTqdcQ
};

#endif
