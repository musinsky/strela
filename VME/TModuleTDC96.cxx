// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    05 Dec 2013

#include "TModuleTDC96.h"
#include "TVME.h"

const Int_t kNChips = 3;
const Int_t kChipNChannels = 32;

// from daqlib/modules/tdc96_types.h (by Ilja Slepnev)
const Int_t kMap[kChipNChannels] = { // tdc_ch
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
};
const Int_t kId2Num[16] = { // tdc_id
  -1, 0, 1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
// connectors enumeration (front panel)
//           2
//           1
//           0
const Int_t kConnector[kNChips] = {31, 63, 95};

ClassImp(TModuleTDC96)

//______________________________________________________________________________
TModuleTDC96::TModuleTDC96()
: TVirtualModule()
{
  //  Info("TModuleTDC96", "Default constructor");
  fId            = 0x05; // 5 in DEC
  fNChips        = kNChips;
  fChipNChannels = kChipNChannels;
}
//______________________________________________________________________________
TModuleTDC96::TModuleTDC96(Int_t slot)
: TVirtualModule()
{
  //  Info("TModuleTDC96", "Normal constructor");
  fId            = 0x05; // 5 in DEC
  fNChips        = kNChips;
  fChipNChannels = kChipNChannels;
  VMEModule(slot);
}
//______________________________________________________________________________
TModuleTDC96::~TModuleTDC96()
{
  Info("~TModuleTDC96", "Destructor");
}
//______________________________________________________________________________
void TModuleTDC96::Print(Option_t *option) const
{
  const Int_t firstch[kNChips] = {
    kConnector[2] - kChipNChannels + 1,
    kConnector[1] - kChipNChannels + 1,
    kConnector[0] - kChipNChannels + 1
  };
  Int_t del = 0, channelE, channelO, nadc, id, ch;
  if (fFirstChannel < 0)
    Warning("Print", "module %s is not in list of modules", GetTitle());
  else del = fFirstChannel;

  for (Int_t i = 0; i < kNChips; i++) {
    Printf("\tGND\n\t GND");
    for (Int_t j = 0; j < kChipNChannels; j+=2) {
      channelE = del + firstch[i] + j;     // even
      channelO = del + firstch[i] + j + 1; // odd

      if (!strcmp(option, "simple"))
        Printf("\t%3d\n\t %3d", channelE, channelO);

      else if (!strcmp(option, "nadc")) {
        if (fSlot < 0) {
          Info("Print", "only for VME module");
          return;
        }
        nadc = gVME->GetChannel()[channelE];
        printf(" (%4d) %3d", nadc, channelE);
        nadc = gVME->GetChannel()[channelO];
        printf("\n\t %3d (%4d)\n", channelO, nadc);
      }

      else { // default
        GetChannelIdCh(channelE - del, id, ch);
        printf("[%2d][%d] %3d", ch, id, channelE);
        GetChannelIdCh(channelO - del, id, ch);
        printf("\n\t %3d [%d][%2d]\n", channelO, id, ch);
      }
    }
    printf("\n");
  }
}
//______________________________________________________________________________
Int_t TModuleTDC96::MapChannel(Int_t tdcid, Int_t tdcch) const
{
  Int_t num = kId2Num[tdcid]; // & 0xF => always < 16;   1, 2, 4
  if (num == -1) {
    Error("MapChannel", "module %s with impossible id: %d", GetTitle(), tdcid);
    return -1;
  }
  Int_t ch = tdcch; // & 0x1F => always < 32;   0, 1, 2, ..., 31
  return num*kChipNChannels + kMap[ch];
}
//______________________________________________________________________________
Bool_t TModuleTDC96::GetChannelIdCh(Int_t ch, Int_t &tdcid, Int_t &tdcch) const
{
  for (Int_t num = 0; num < kNChips; num++) {
    for (Int_t ch2 = 0; ch2 < kChipNChannels; ch2++) {
      tdcid = 1 << num;
      tdcch = ch2;
      if (MapChannel(tdcid, tdcch) == ch) return kTRUE;
    }
  }
  tdcid = -1;
  tdcch = -1;
  return kFALSE;
}
//______________________________________________________________________________
void TModuleTDC96::ConnectorChannels(Int_t con, Int_t *pins, Option_t *option) const
{
  TVirtualModule::ConnectorChannels(con, pins); // only checks
  Int_t count = 0;
  Int_t shift = 0, step = 1;        // all 32 channels
  TString opt = option;
  opt.ToLower();
  if (opt.Contains("even")) {       // 16 "even" channels
    shift = 1;
    step  = 2;
  }
  else if (opt.Contains("odd")) {   // 16 "odd" channels
    shift = 0;
    step  = 2;
  }
  else
    Warning("ConnectorChannels", "double connect on module %s", GetTitle());

  // order of channels: first, ..., last, GND
  for (Int_t i = 0; i < kChipNChannels; i+=step)
    pins[count++] = gVME->GetChannel()[fFirstChannel + kConnector[con] - shift - i];
}
