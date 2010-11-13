// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 17 Jun 2008

#ifndef STRELA_TModuleTDC96
#define STRELA_TModuleTDC96

#ifndef STRELA_TVirtualModule
#include <TVirtualModule.h>
#endif

class TModuleTDC96 : public TVirtualModule {

public:
  TModuleTDC96();
  TModuleTDC96(const char *name, const char *title = "TDC-96 V1.0");
  virtual       ~TModuleTDC96();

  virtual void   Print(Option_t *option = "") const;
  virtual Int_t  MapChannel(Int_t tdcid, Int_t tdcch) const;
  virtual Bool_t GetChannelIdCh(Int_t ch, Int_t &tdcid, Int_t &tdcch) const;
  virtual void   ConnectorChannels(Int_t con, Int_t *pins,
                                   Option_t *option) const;

  ClassDef(TModuleTDC96, 1) // TDC96 module (96 channels)
};

#endif
