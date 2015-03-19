// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Mar 2015

#ifndef STRELA_TVirtualHit
#define STRELA_TVirtualHit

#include <TObject.h>

class TVirtualHit : public TObject {

public:
  TVirtualHit() : TObject(), fChannel(0) {}
  virtual ~TVirtualHit() {}

  Int_t         GetChannel() const { return fChannel; }

protected:
  UShort_t      fChannel; // channel (5 bits word)

  ClassDef(TVirtualHit, 1) // Hit(virtual) class
};

#endif
