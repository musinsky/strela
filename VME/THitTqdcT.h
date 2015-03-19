// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Mar 2015

#ifndef STRELA_THitTqdcT
#define STRELA_THitTqdcT

#include <TVirtualHit.h>

class THitTqdcT : public TVirtualHit {

public:
  THitTqdcT() : TVirtualHit(), fT(0) {}
  virtual ~THitTqdcT() {}

  Int_t         GetChannelTqdcT() const { return GetChannel(); }
  Int_t         GetT() const { return fT; }
  void          Set(Int_t ch, Int_t t) { fChannel = ch; fT = t; }

private:
  UShort_t      fT; // time (19 or 21 bits word)

  ClassDef(THitTqdcT, 1) // HitTqdcT
};

#endif
