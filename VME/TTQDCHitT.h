// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    10 Mar 2015

#ifndef STRELA_TTQDCHitT
#define STRELA_TTQDCHitT

#include <TObject.h>

class TTQDCHitT : public TObject {

public:
  TTQDCHitT() : TObject(), fChannelT(0), fT(0) {}
  virtual ~TTQDCHitT() {}

  Int_t         GetChannelT() const { return fChannelT; }
  Int_t         GetT() const { return fT; }
  void          Set(Int_t ch, Int_t t) { fChannelT = ch; fT = t; }

private:
  UShort_t      fChannelT; // channel
  UShort_t      fT;        // time (19 or 21 bits word)

  ClassDef(TTQDCHitT, 1) // TQDCHitT
};

#endif
