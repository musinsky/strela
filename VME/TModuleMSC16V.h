// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    17 Nov 2016

#ifndef STRELA_TModuleMSC16V
#define STRELA_TModuleMSC16V

#include "TVirtualModule.h"

class TModuleMSC16V : public TVirtualModule {

public:
  TModuleMSC16V();
  TModuleMSC16V(Int_t slot);
  virtual ~TModuleMSC16V();

  virtual void   Print(Option_t *option = "") const;
  virtual Bool_t IsMSC() const { return kTRUE; }

  ClassDef(TModuleMSC16V, 1) // MSC16V module (16 channels)
};

#endif
