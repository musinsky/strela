// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Mar 2015

#ifndef STRELA_THitTdc
#define STRELA_THitTdc

#include "TVirtualHit.h"

class THitTdc : public TVirtualHit {

public:
  THitTdc() : TVirtualHit(), fTime(0), fDelta(0) {}
  virtual ~THitTdc() {}

  Int_t         GetChannelTdc() const { return GetChannel(); }
  Int_t         GetTime() const { return fTime; }
  Int_t         GetDelta() const { return fDelta; }
  void          Set(Int_t ch, Int_t tld);
  void          SetDelta(Int_t ttr);
  Int_t         GetTLD() const { return fTime; }
  Int_t         GetTTR() const { return fDelta ? fTime + fDelta : 0; }

private:
  UShort_t      fTime;  // TDC leading time (19 or 21 bits word)
  UShort_t      fDelta; // TDC delta time

  ClassDef(THitTdc, 1) // HitTdc
};

#endif
