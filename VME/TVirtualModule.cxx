// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    11 Nov 2010

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
TVirtualModule::TVirtualModule(const char *name, const char *title) : TNamed(name, title)
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
Int_t TVirtualModule::MapChannel(Int_t tdcid, Int_t tdcch) const
{
  // override this function
  return -1;
}
//______________________________________________________________________________
Bool_t TVirtualModule::GetChannelIdCh(Int_t ch, Int_t &tdcid, Int_t &tdcch) const
{
  // override this function
  return kFALSE;
}
//______________________________________________________________________________
void TVirtualModule::ConnectorChannels(Int_t con, Int_t *pins, Option_t *option) const
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
