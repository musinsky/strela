// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    30 Nov 2013

#ifndef STRELA_TModulePhTDC
#define STRELA_TModulePhTDC

#ifndef STRELA_TVirtualModule
#include <TVirtualModule.h>
#endif

class TModulePhTDC : public TVirtualModule {

public:
  TModulePhTDC();
  TModulePhTDC(Int_t slot);
  virtual       ~TModulePhTDC();

  virtual void   Print(Option_t *option = "") const;
  virtual Int_t  MapChannel(Int_t tdcid, Int_t tdcch) const;
  virtual Bool_t GetChannelIdCh(Int_t ch, Int_t &tdcid, Int_t &tdcch) const;
  virtual void   ConnectorChannels(Int_t con, Int_t *pins, Option_t *option) const;

  ClassDef(TModulePhTDC, 1) // PhTDC module (64 channels)
};

#endif
