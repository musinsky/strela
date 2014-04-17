// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    17 Apr 2014

#ifndef STRELA_TTDCHit
#define STRELA_TTDCHit

#include <TObject.h>

class TTDCHit : public TObject {

public:
  TTDCHit() : TObject(), fChannel(0), fTime(0), fDelta(0) {}
  TTDCHit(Int_t ch, Int_t tld) : TObject(), fChannel(ch), fTime(tld), fDelta(0) {}
  virtual ~TTDCHit() {}

  Int_t         GetChannel() const { return fChannel; }
  Int_t         GetTime() const { return fTime; }
  Int_t         GetDelta() const { return fDelta; }
  void          SetDelta(Int_t ttr);
  Int_t         GetTLD() const { return fTime; }
  Int_t         GetTTR() const { return fDelta ? fTime + fDelta : 0; }

private:
  UShort_t      fChannel; // channel
  UShort_t      fTime;    // leading time
  UShort_t      fDelta;   // delta time

  ClassDef(TTDCHit, 1) // TDCHit
};

#endif