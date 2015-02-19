// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Feb 2015

#include "TModuleTQDC16.h"

ClassImp(TModuleTQDC16)

//______________________________________________________________________________
TModuleTQDC16::TModuleTQDC16()
: TVirtualModule()
{
  //  Info("TModuleTQDC16", "Default constructor");
  fId            = 0x09; // 9 in DEC
  fNChips        = 2;
  fChipNChannels = 8;
}
//______________________________________________________________________________
TModuleTQDC16::TModuleTQDC16(Int_t slot)
: TVirtualModule()
{
  //  Info("TModuleTQDC16", "Normal constructor");
  fId            = 0x09; // 9 in DEC
  fNChips        = 2;
  fChipNChannels = 8;
  VMEModule(slot);
}
//______________________________________________________________________________
TModuleTQDC16::~TModuleTQDC16()
{
  Info("~TModuleTQDC16", "Destructor");
}
//______________________________________________________________________________
void TModuleTQDC16::Print(Option_t * /*option*/) const
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
