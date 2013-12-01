// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    01 Dec 2013

#include "TModuleTDC64V.h"
#include "TVME.h"

const Int_t kNChips = 2;
const Int_t kChipNChannels = 32;

// from daqlib/modules/tdc64v_types.h (by Ilja Slepnev)
const Int_t kMap[kNChips][kChipNChannels] = { // tdc_num, tdc_ch
  {
    31, 15, 30, 14, 13, 29, 28, 12, 11, 27, 26, 10, 25,  9, 24,  8,
    23,  7, 22,  6, 21,  5, 20,  4, 19,  3, 18,  2, 17,  1, 16,  0
  }, {
    /*
    31, 15, 30, 14, 29, 13, 28, 12, 27, 11, 26, 10, 25,  9, 24,  8,
    23,  7, 22,  6, 21,  5, 20,  4, 19,  3, 18,  2, 17,  1, 16,  0
     */
    63, 47, 62, 46, 61, 45, 60, 44, 59, 43, 58, 42, 57, 41, 56, 40,
    55, 39, 54, 38, 53, 37, 52, 36, 51, 35, 50, 34, 49, 33, 48, 32
  }
};
const Int_t kId2Num[16] = { // tdc_id
  -1, 0, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
// connectors enumeration (front panel)
//        3   2      => HPTDC Id = 2 (tdc_num = 1)
//        1   0      => HPTDC Id = 1 (tdc_num = 0)
const Int_t kConnector[kNChips*2] = { 0, 16, 32, 48};

ClassImp(TModuleTDC64V)

//______________________________________________________________________________
TModuleTDC64V::TModuleTDC64V()
{
  //  Info("TModuleTDC64V", "Default constructor");
  fId            = 0x10; // 16 in DEC
  fNChips        = kNChips;
  fChipNChannels = kChipNChannels;
}
//______________________________________________________________________________
TModuleTDC64V::TModuleTDC64V(Int_t slot) : TVirtualModule(slot)
{
  //  Info("TModuleTDC64V", "Normal constructor");
  fId            = 0x10; // 16 in DEC
  fNChips        = kNChips;
  fChipNChannels = kChipNChannels;
}
//______________________________________________________________________________
TModuleTDC64V::~TModuleTDC64V()
{
  Info("~TModuleTDC64V", "Destructor");
}
//______________________________________________________________________________
void TModuleTDC64V::Print(Option_t *option) const
{
  const Int_t firstch[kNChips*2] = {
    kConnector[3] + kChipNChannels/2 - 1, kConnector[2] + kChipNChannels/2 - 1,
    kConnector[1] + kChipNChannels/2 - 1, kConnector[0] + kChipNChannels/2 - 1
  };
  Int_t del = 0, channelL, channelR, nadc, id, ch;
  if (gVME) {
    del = gVME->FirstChannelOfModule(this);
    if (del < 0) {
      del = 0;
      Warning("Print", "module %s is not in list of modules", GetTitle());
    }
  }

  for (Int_t i = 0; i < (kNChips*2); i+=2) {
    Printf("GND\t\tGND");
    for (Int_t j = 0; j < (kChipNChannels/2); j++) {
      channelL = del + firstch[i] - j;   // left
      channelR = del + firstch[i+1] - j; // right

      if (!strcmp(option, "simple"))
        Printf("%3d\t\t%3d", channelL, channelR);

      else if (!strcmp(option, "nadc")) {
        if (!gVME || !gVME->GetChannel()) {
          Info("Print", "only after ReDecodeChannels");
          return;
        }
        nadc = gVME->GetChannel()[channelL];
        printf("(%4d) %3d", nadc, channelL);
        nadc = gVME->GetChannel()[channelR];
        printf("\t%3d (%4d)\n", channelR, nadc);
      }

      else { // default
        GetChannelIdCh(channelL - del, id, ch);
        printf("[%2d][%d] %3d", ch, id, channelL);
        GetChannelIdCh(channelR - del, id, ch);
        printf("\t%3d [%d][%2d]\n", channelR, id, ch);
      }
    }
    printf("\n");
  }
}
//______________________________________________________________________________
Int_t TModuleTDC64V::MapChannel(Int_t tdcid, Int_t tdcch) const
{
  Int_t num = kId2Num[tdcid]; // & 0xF => always < 16;   1, 2
  if (num == -1) {
    Error("MapChannel", "module %s with impossible id: %d", GetTitle(), tdcid);
    return -1;
  }
  Int_t ch = tdcch; // & 0x1F => always < 32;   31, 15, 30, ..., 0
  return kMap[num][ch];
}
//______________________________________________________________________________
Bool_t TModuleTDC64V::GetChannelIdCh(Int_t ch, Int_t &tdcid, Int_t &tdcch) const
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
void TModuleTDC64V::ConnectorChannels(Int_t con, Int_t *pins, Option_t * /*option*/) const
{
  TVirtualModule::ConnectorChannels(con/2, pins); // only checks
  Int_t del = gVME->FirstChannelOfModule(this);

  // order of channels: first, ..., last, GND
  for (Int_t i = 0; i < (kChipNChannels/2); i++)
    pins[i] = gVME->GetChannel()[del + kConnector[con] + i];
}
