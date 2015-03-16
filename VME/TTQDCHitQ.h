// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    16 Mar 2015

#ifndef STRELA_TTQDCHitQ
#define STRELA_TTQDCHitQ

#include <TObject.h>

class TTQDCHitQ : public TObject {

public:
  TTQDCHitQ();
  virtual ~TTQDCHitQ();

  Int_t         GetChannelQ() const { return fChannelQ; }
  Int_t         GetQ() const { return fQ; }
  Int_t         GetTimeStamp() const { return fTimeStamp; }
  Int_t         GetNSamples() const { return fNSamples; }
  Short_t      *GetSample() { return fSample; }
  void          Set(Int_t ch, Int_t ts);
  void          NextSample(Int_t sample);

private:
  UShort_t      fChannelQ;  // channel
  Int_t         fQ;         // charge (sum of samples)
  Short_t       fTimeStamp; // timestamp (ADC ts - TRIG ts)
  Int_t         fNSamples;  // number of samples
  Short_t      *fSample;    //[fNSamples] array of samples (14 bits word, signed)

  Int_t         fMaxSamples; //! maximum number of samples
  void          ExpandSamples(Int_t nsize);

  ClassDef(TTQDCHitQ, 1) // TQDCHitQ
};

#endif
