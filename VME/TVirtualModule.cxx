// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    13 Apr 2014

#include "TVirtualModule.h"
#include "TVME.h"

ClassImp(TVirtualModule)

//______________________________________________________________________________
TVirtualModule::TVirtualModule()
: TObject(),
  fId(-1),
  fSlot(-1),
  fNChips(0),
  fChipNChannels(0),
  fFirstChannel(-1)
{
  // Default constructor
}
//______________________________________________________________________________
TVirtualModule::~TVirtualModule()
{
  Info("~TVirtualModule", "Destructor");
  if (gVME) {
    gVME->Modules()->Remove(this);
    gVME->ReDecodeChannels();
  }
}
//______________________________________________________________________________
const char *TVirtualModule::GetTitle() const
{
  return Form("%s (Id: %02d) in slot: %02d", GetName(), fId, fSlot);
}
//______________________________________________________________________________
void TVirtualModule::VMEModule(Int_t slot)
{
  if (!gVME) {
    Error("VMEModule", "gVME not initialized");
    return;
  }
  if (gVME->Modules()->At(slot)) {
    Error("VMEModule", "slot %d is already occupied", slot);
    return;
  }

  fSlot = slot;
  gVME->Modules()->AddAt(this, fSlot);
  gVME->ReDecodeChannels(); // and SetFirstChannel for ! each ! modules
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
  if ((fFirstChannel < 0) || (fSlot < 0))
    Error("ConnectorChannels", "module %s is not in list of modules", GetTitle());
  if (con >= fNChips)
    Error("ConnectorChannels", "module %s has only %d chips", GetTitle(), fNChips);
}
