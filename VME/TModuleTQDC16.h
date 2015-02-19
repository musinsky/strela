// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Feb 2015

#ifndef STRELA_TModuleTQDC16
#define STRELA_TModuleTQDC16

#include "TVirtualModule.h"

class TModuleTQDC16 : public TVirtualModule {

public:
  TModuleTQDC16();
  TModuleTQDC16(Int_t slot);
  virtual ~TModuleTQDC16();

  virtual void   Print(Option_t *option = "") const;
  virtual Bool_t IsTQDC() const { return kTRUE; }

  ClassDef(TModuleTQDC16, 1) // TQDC16 module (16 channels)
};

#endif
