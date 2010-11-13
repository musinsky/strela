// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 18 Jun 2008

#include "TVirtualModule.h"
#include "TVME.h"

ClassImp(TVirtualModule)

//______________________________________________________________________________
TVirtualModule::TVirtualModule()
{
  //  Info("TVirtualModule", "Default constructor");
  fNChips        = 0;
  fChipNChannels = 0;
}
//______________________________________________________________________________
TVirtualModule::TVirtualModule(const char *name, const char *title)
  : TNamed(name, title)
{
  //  Info("TVirtualModule", "Normal constructor");
  fNChips        = 0;
  fChipNChannels = 0;
  if (gVME) gVME->Modules()->Add(this);
}
//______________________________________________________________________________
TVirtualModule::~TVirtualModule()
{
  Info("~TVirtualModule", "Destructor");
  if (gVME) gVME->Modules()->Remove(this);
}
//______________________________________________________________________________
void TVirtualModule::ConnectorChannels(Int_t con, Int_t *pins,
                                       Option_t *option) const
{
  if (gVME->GetNChannels() != gVME->GetNChannelsFast())
    Error("ConnectorChannels", "must ReDecode all channels");
  if ((gVME->FirstChannelOfModule(this) < 0) || (con >= fNChips))
    Error("ConnectorChannels", "problem with module %s", GetName());
}
//______________________________________________________________________________
Int_t TVirtualModule::DecodeChannel(UInt_t word) const
{
  if ((word >> 28) != 0x4) {
    Warning("DecodeChannel", "decode only DATA(0x4), not: 0x%X", word >> 28);
    return -1;
  }
  // tdc_id = bits 24 - 27, tdc_ch = bits 19 - 23
  return MapChannel((word >> 24) & 0xF, (word >> 19) & 0x1F);
}
