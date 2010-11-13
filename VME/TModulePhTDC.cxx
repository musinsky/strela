// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 26 Oct 2010

#include "TModulePhTDC.h"
#include "TVME.h"

const Int_t kNChips = 4;
const Int_t kChipNChannels = 16;

// from daq7300/decoder/decode_phtdc.c (by Ilja Slepnev)
const Int_t kMap[kNChips][kChipNChannels] = { // tdc_num, tdc_ch
  {
    0x17, 0x16, 0x07, 0x06,
    0x15, 0x14, 0x05, 0x04,
    0x13, 0x12, 0x03, 0x02,
    0x11, 0x10, 0x01, 0x00
  }, {
    0x1F, 0x1E, 0x0F, 0x0E,
    0x1D, 0x1C, 0x0D, 0x0C,
    0x1B, 0x1A, 0x0B, 0x0A,
    0x19, 0x18, 0x09, 0x08
  }, {
    0x37, 0x36, 0x27, 0x26,
    0x35, 0x34, 0x25, 0x24,
    0x33, 0x32, 0x23, 0x22,
    0x31, 0x30, 0x21, 0x20
  }, {
    0x3F, 0x3E, 0x2F, 0x2E,
    0x3D, 0x3C, 0x2D, 0x2C,
    0x3B, 0x3A, 0x2B, 0x2A,
    0x39, 0x38, 0x29, 0x28
  }
};
const Int_t kId2Num[16] = { // tdc_id
  -1, 0, 1, -1, 2, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1
};
// connectors enumeration (front panel)
//        2   3
//        0   1
const Int_t kConnector[kNChips] = { 0, 16, 32, 48};

ClassImp(TModulePhTDC)

//______________________________________________________________________________
TModulePhTDC::TModulePhTDC()
{
  //  Info("TModulePhTDC", "Default constructor");
  fNChips        = kNChips;
  fChipNChannels = kChipNChannels;
}
//______________________________________________________________________________
TModulePhTDC::TModulePhTDC(const char *name, const char *title)
  : TVirtualModule(name, title)
{
  //  Info("TModulePhTDC", "Normal constructor");
  fNChips        = kNChips;
  fChipNChannels = kChipNChannels;
}
//______________________________________________________________________________
TModulePhTDC::~TModulePhTDC()
{
  Info("~TModulePhTDC", "Destructor");
}
//______________________________________________________________________________
void TModulePhTDC::Print(Option_t *option) const
{
  const Int_t firstch[kNChips] = {
    kConnector[2] + kChipNChannels - 1, kConnector[3] + kChipNChannels - 1,
    kConnector[0] + kChipNChannels - 1, kConnector[1] + kChipNChannels - 1
  };
  Int_t del = 0, channelL, channelR, nadc, id, ch;
  if (gVME) {
    del = gVME->FirstChannelOfModule(this);
    if (del < 0) {
      del = 0;
      Warning("Print", "module %s is not in list of modules", GetName());
    }
  }

  for (Int_t i = 0; i < kNChips; i+=2) {
    Printf("GND\t\tGND");
    for (Int_t j = 0; j < kChipNChannels; j++) {
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
        printf("%3d (%4d)", channelL, nadc);
        nadc = gVME->GetChannel()[channelR];
        printf("\t%3d (%4d)\n", channelR, nadc);
      }

      else { // default
        GetChannelIdCh(channelL - del, id, ch);
        printf("%3d [%d][%2d]", channelL, id, ch);
        GetChannelIdCh(channelR - del, id, ch);
        printf("\t%3d [%d][%2d]\n", channelR, id, ch);
      }
    }
    printf("\n");
  }
}
//______________________________________________________________________________
Int_t TModulePhTDC::MapChannel(Int_t tdcid, Int_t tdcch) const
{
  Int_t num = kId2Num[tdcid]; // & 0xF => always < 16;   1, 2, 4, 8
  if (num == -1) {
    Error("MapChannel", "module %s with impossible id: %d", GetName(), tdcid);
    return -1;
  }
  if ((tdcch%2) == 1) {
    Error("MapChannel", "module %s with impossible ch: %d", GetName(), tdcch);
    return -1;
  }
  Int_t ch = tdcch/2; // & 0x1F => always < 32;   0, 2, 4, ..., 30
  return kMap[num][ch];
}
//______________________________________________________________________________
Bool_t TModulePhTDC::GetChannelIdCh(Int_t ch, Int_t &tdcid, Int_t &tdcch) const
{
  for (Int_t num = 0; num < kNChips; num++) {
    for (Int_t ch2 = 0; ch2 < kChipNChannels; ch2++) {
      tdcid = 1 << num;
      tdcch = 2*ch2;
      if (MapChannel(tdcid, tdcch) == ch) return kTRUE;
    }
  }
  tdcid = -1;
  tdcch = -1;
  return kFALSE;
}
//______________________________________________________________________________
void TModulePhTDC::ConnectorChannels(Int_t con, Int_t *pins,
                                     Option_t *option) const
{
  TVirtualModule::ConnectorChannels(con, pins); // only checks
  Int_t del = gVME->FirstChannelOfModule(this);

  // order of channels: first, ..., last, GND
  for (Int_t i = 0; i < kChipNChannels; i++)
    pins[i] = gVME->GetChannel()[del + kConnector[con] + i];
}
