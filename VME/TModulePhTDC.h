// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 17 Jun 2008

#ifndef STRELA_TModulePhTDC
#define STRELA_TModulePhTDC

#ifndef STRELA_TVirtualModule
#include <TVirtualModule.h>
#endif

class TModulePhTDC : public TVirtualModule {

public:
  TModulePhTDC();
  TModulePhTDC(const char *name, const char *title = "PhTDC - V1.1");
  virtual       ~TModulePhTDC();

  virtual void   Print(Option_t *option = "") const;
  virtual Int_t  MapChannel(Int_t tdcid, Int_t tdcch) const;
  virtual Bool_t GetChannelIdCh(Int_t ch, Int_t &tdcid, Int_t &tdcch) const;
  virtual void   ConnectorChannels(Int_t con, Int_t *pins,
                                   Option_t *option) const;

  ClassDef(TModulePhTDC, 1) // PhTDC module (64 channels)
};

#endif
