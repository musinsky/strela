// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    17 Nov 2016

#include "TModuleMSC16V.h"

ClassImp(TModuleMSC16V)

//______________________________________________________________________________
TModuleMSC16V::TModuleMSC16V()
: TVirtualModule()
{
  //  Info("TModuleMSC16V", "Default constructor");
  fId            = 0x0F; // 15 in DEC
  fNChips        = 1;
  fChipNChannels = 16;
}
//______________________________________________________________________________
TModuleMSC16V::TModuleMSC16V(Int_t slot)
: TVirtualModule()
{
  //  Info("TModuleMSC16V", "Normal constructor");
  fId            = 0x0F; // 15 in DEC
  fNChips        = 1;
  fChipNChannels = 16;
  VMEModule(slot);
}
//______________________________________________________________________________
TModuleMSC16V::~TModuleMSC16V()
{
  Info("~TModuleMSC16V", "Destructor");
}
//______________________________________________________________________________
void TModuleMSC16V::Print(Option_t * /*option*/) const
{
  Int_t del = 0;
  if (fFirstChannel < 0)
    Warning("Print", "module %s is not in list of modules", GetTitle());
  else del = fFirstChannel;

  printf("--GND--\n");
  for (Int_t i = 0; i < GetModuleNChannels(); i++) {
    if (i == fChipNChannels) printf("-------\n");
    printf("%02d [%02d]\n", del + i, i);
  }
}
