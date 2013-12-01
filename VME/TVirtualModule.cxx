// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    01 Dec 2013

#include "TVirtualModule.h"
#include "TVME.h"

ClassImp(TVirtualModule)

//______________________________________________________________________________
TVirtualModule::TVirtualModule()
{
  //  Info("TVirtualModule", "Default constructor");
  fId            = -1;
  fSlot          = -1;
  fNChips        = 0;
  fChipNChannels = 0;
}
//______________________________________________________________________________
TVirtualModule::TVirtualModule(Int_t slot) : TObject()
{
  //  Info("TVirtualModule", "Normal constructor");
  fId            = -1;   // http://afi.jinr.ru/VmeModuleId
  fSlot          = slot;
  fNChips        = 0;
  fChipNChannels = 0;
  if (!gVME) return;

  if (gVME->Modules()->At(slot)) {
    Error("TVirtualModule", "slot %d is already occupied", slot);
    fSlot = -1;
  }
  else gVME->Modules()->AddAt(this, slot);
}
//______________________________________________________________________________
TVirtualModule::~TVirtualModule()
{
  Info("~TVirtualModule", "Destructor");
  if (gVME) gVME->Modules()->Remove(this);
}
//______________________________________________________________________________
const char *TVirtualModule::GetTitle() const
{
  return Form("%s (Id: %02d) in slot: %02d", GetName(), fId, fSlot);
}
//______________________________________________________________________________
Int_t TVirtualModule::MapChannel(Int_t /*tdcid*/, Int_t /*tdcch*/) const
{
  // override this function
  return -1;
}
//______________________________________________________________________________
Bool_t TVirtualModule::GetChannelIdCh(Int_t /*ch*/, Int_t & /*tdcid*/, Int_t & /*tdcch*/) const
{
  // override this function
  return kFALSE;
}
//______________________________________________________________________________
void TVirtualModule::ConnectorChannels(Int_t con, Int_t * /*pins*/, Option_t * /*option*/) const
{
  if (gVME->GetNChannels() != gVME->GetNChannelsFast())
    Error("ConnectorChannels", "must ReDecode all channels");
  if ((gVME->FirstChannelOfModule(this) < 0) || (con >= fNChips))
    Error("ConnectorChannels", "problem with module %s", GetName());
}
//______________________________________________________________________________
Int_t TVirtualModule::DecodeChannel(UInt_t word) const
{
  UInt_t word28 = word >> 28;
  if ((word28 != 0x4) && (word28 != 0x5)) {
    Warning("DecodeChannel", "decode only DATA (0x4 or 0x5), not: 0x%X", word28);
    return -1;
  }
  // tdc_id = bits 24 - 27, tdc_ch = bits 19 - 23
  return MapChannel((word >> 24) & 0xF, (word >> 19) & 0x1F);
}
