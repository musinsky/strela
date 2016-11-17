// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    17 Nov 2016

#ifndef STRELA_THitTqdcT
#define STRELA_THitTqdcT

#include "TVirtualHit.h"

class THitTqdcT : public TVirtualHit {

public:
  THitTqdcT() : TVirtualHit(), fTime(0), fDelta(0) {}
  virtual ~THitTqdcT() {}

  Int_t         GetChannelTqdcT() const { return GetChannel(); }
  Int_t         GetTime() const { return fTime; }
  Int_t         GetDelta() const { return fDelta; }
  void          Set(Int_t ch, Int_t tld);
  void          SetDelta(Int_t ttr);
  Int_t         GetTLD() const { return fTime; }
  Int_t         GetTTR() const { return fDelta ? fTime + fDelta : 0; }

private:
  UShort_t      fTime;  // TQDC leading time (19 or 21 bits word)
  UShort_t      fDelta; // TQDC delta time

  ClassDef(THitTqdcT, 1) // HitTqdcT
};

#endif
