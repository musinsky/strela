// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    30 Nov 2013

#ifndef STRELA_TModuleTDC64V
#define STRELA_TModuleTDC64V

#ifndef STRELA_TVirtualModule
#include <TVirtualModule.h>
#endif

class TModuleTDC64V : public TVirtualModule {

public:
  TModuleTDC64V();
  TModuleTDC64V(Int_t slot);
  virtual       ~TModuleTDC64V();

  virtual void   Print(Option_t *option = "") const;
  virtual Int_t  MapChannel(Int_t tdcid, Int_t tdcch) const;
  virtual Bool_t GetChannelIdCh(Int_t ch, Int_t &tdcid, Int_t &tdcch) const;
  virtual void   ConnectorChannels(Int_t con, Int_t *pins, Option_t *option) const;

  ClassDef(TModuleTDC64V, 1) // TDC64V module (64 channels)
};

#endif
