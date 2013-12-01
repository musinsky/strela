// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    02 Dec 2013

#include <fcntl.h>
#include <TError.h>

#include "TRawData.h"
#include "TVME.h"
#include "TVirtualModule.h"

const UInt_t kTHDR = 0x2; // TDC event header
const UInt_t kTTRL = 0x3; // TDC event trailer
const UInt_t kTLD  = 0x4; // TDC leading
const UInt_t kTTR  = 0x5; // TDC trailing
const UInt_t kTERR = 0x6; // TDC error

const UInt_t kMHDR = 0x8; // Module header
const UInt_t kMTRL = 0x9; // Module trailer
const UInt_t kEHDR = 0xA; // Event header
const UInt_t kETRL = 0xB; // Event trailer
const UInt_t kSHDR = 0xC; // Spill header
const UInt_t kSTRL = 0xD; // Spill trailer
const UInt_t kSTAT = 0xE; // Status
const UInt_t kRESE = 0xF; // Reserved

ClassImp(TRawData)

//______________________________________________________________________________
TRawData::TRawData()
: fWord(0),
  fFile(0),
  fNWords(0),
  fEvent(-1),
  fTdcId(-1),
  fModule(0)
{
  // Default constructor
}
//______________________________________________________________________________
TRawData::~TRawData()
{
  // Destructor
}
//______________________________________________________________________________
void TRawData::ParseFile(const char *fname)
{
  fFile = fopen(fname, "rb");
  if (!fFile) {
    Error("ParseFile", "file %s can not be opened", fname);
    return;
  }

  fNWords = 0;
  while (fread(&fWord, sizeof(fWord), 1, fFile) == 1) {
    FindType();
    fNWords++;
  }

  fclose(fFile);
}
//______________________________________________________________________________
void TRawData::FindType()
{
  UInt_t type = fWord >> 28; // (bits 28 - 31)
  switch (type) {
    case kTHDR:
      DecodeTHDR();
      return;
    case kTTRL:
      DecodeTTRL();
      return;
    case kTLD:
      DecodeTLD();
      return;
    case kTTR:
      DecodeTTR();
      return;
    case kTERR:
      DecodeTERR();
      return;
    case kMHDR:
      DecodeMHDR();
      return;
    case kMTRL:
      DecodeMTRL();
      return;
    case kEHDR:
      DecodeEHDR();
      return;
    case kETRL:
      DecodeETRL();
      return;
    case kSHDR:
      DecodeSHDR();
      return;
    case kSTRL:
      DecodeSTRL();
      return;
    case kSTAT:
      DecodeSTAT();
      return;
    case kRESE:
      DecodeRESE();
      return;
    default:
      DecodeOther();
      return;
  }
}
//______________________________________________________________________________
void TRawData::DecodeTHDR()
{
  if (!fModule) {
    PrintWord(3);
    printf("noTDC\n");
    return;
  }

  // 0x2 TDC event header
  Int_t ts = fWord & 0xFFF;         // (bits 0  - 11)
  Int_t ev = (fWord >> 12) & 0xFFF; // (bits 12 - 23)
  fTdcId   = (fWord >> 24) & 0xF;   // (bits 24 - 27)

  if (ev != fEvent) Error("DecodeTHDR", "event mismatch %d != %d", ev, fEvent);

  PrintWord(3);
  printf("THDR ts: %d, ev: %d, id: %2d\n", ts, ev, fTdcId);
}
//______________________________________________________________________________
void TRawData::DecodeTTRL()
{
  if (!fModule) {
    PrintWord(3);
    printf("noTDC\n");
    return;
  }

  // 0x3 TDC event trailer
  Int_t wc = fWord & 0xFFF;         // (bits 0  - 11)
  Int_t ev = (fWord >> 12) & 0xFFF; // (bits 12 - 23)
  Int_t id = (fWord >> 24) & 0xF;   // (bits 24 - 27)

  if (ev != fEvent) Error("DecodeTTRL", "event mismatch %d != %d", ev, fEvent);
  if (id != fTdcId) Error("DecodeTTRL", "tdc_id mismatch %d != %d", id, fTdcId);

  PrintWord(3);
  printf("TTRL wc: %d, ev: %d, id: %2d\n", wc, ev, id);
}
//______________________________________________________________________________
void TRawData::DecodeTLD()
{
  if (!fModule) {
    PrintWord(4);
    printf("noTDC\n");
    return;
  }

  // 0x4 TDC leading
  Int_t tdc = fWord & 0x7FFFF;      // (bits 0  - 18)
  Int_t ch  = (fWord >> 19) & 0x1F; // (bits 19 - 23)
  Int_t id  = (fWord >> 24) & 0xF;  // (bits 24 - 27)

  if (id != fTdcId) Error("DecodeTLD", "tdc_id mismatch %d != %d", id, fTdcId);

  PrintWord(4);
  printf("TLD tdc: %4d, ch: %2d, id: %2d (nadc: %02d)\n", tdc, ch, id, fModule->MapChannel(id, ch));
}
//______________________________________________________________________________
void TRawData::DecodeTTR()
{
  if (!fModule) {
    PrintWord(4);
    printf("noTDC\n");
    return;
  }

  // 0x5 TDC trailing
  Int_t tdc = fWord & 0x7FFFF;      // (bits 0  - 18)
  Int_t ch  = (fWord >> 19) & 0x1F; // (bits 19 - 23)
  Int_t id  = (fWord >> 24) & 0xF;  // (bits 24 - 27)

  if (id != fTdcId) Error("DecodeTTR", "tdc_id mismatch %d != %d", id, fTdcId);

  PrintWord(4);
  printf("TTR tdc: %4d, ch: %2d, id: %2d (nadc: %02d)\n", tdc, ch, id, fModule->MapChannel(id, ch));
}
//______________________________________________________________________________
void TRawData::DecodeTERR()
{
  // 0x6 TDC error
  Int_t flag = fWord & 0x7FFF;         // (bits 0  - 14)

  PrintWord(3);
  printf("TERR flag: 0x%X\n", flag);
}
//______________________________________________________________________________
void TRawData::DecodeMHDR()
{
  // 0x8 Module header
  Int_t ev = fWord & 0xFFFF;       // (bits 0  - 15)
  Int_t id = (fWord >> 16) & 0x7F; // (bits 16 - 22)
  Int_t ga = (fWord >> 23) & 0x1F; // (bits 23 - 27)

  if (ev != fEvent) Error("DecodeMHDR", "event mismatch %d != %d", ev, fEvent);
  if (gVME) {
    if (ga < gVME->Modules()->GetSize())
      fModule = gVME->GetModule(ga);
  }
  if (fModule && (fModule->GetId() != id))
    Error("DecodeMHDR", "moduleID mismatch %d != %d", id, fModule->GetId());

  PrintWord(2);
  printf("MHDR ev: %d, id: %2d, ga: %2d\n", ev, id, ga);
}
//______________________________________________________________________________
void TRawData::DecodeMTRL()
{
  // 0x9 Module trailer
  Int_t wc  = fWord & 0xFFFF;       // (bits 0  - 15)
  Int_t er  = (fWord >> 16) & 0xF;  // (bits 16 - 19)
  Int_t crc = (fWord >> 20) & 0xFF; // (bits 20 - 27)

  if (er != 0xF) Error("DecodeMTRL", "module error 0x%X", er);

  PrintWord(2);
  printf("MTRL wc: %d, er: 0x%X, crc: 0x%X\n", wc, er, crc);
}
//______________________________________________________________________________
void TRawData::DecodeEHDR()
{
  // 0xA Event header
  fEvent = fWord & 0xFFFFF; // (bits 0 - 19)

  PrintWord(1);
  printf("EHDR ev: %d\n", fEvent);
}
//______________________________________________________________________________
void TRawData::DecodeETRL()
{
  // 0xB Event trailer
  Int_t wc = fWord & 0xFFFFFF; // (bits 0 - 23)

  PrintWord(1);
  printf("ETRL wc: %d\n", wc);
}
//______________________________________________________________________________
void TRawData::DecodeSHDR()
{
  // 0xC Spill header
  Bool_t isEnd = (fWord >> 27) & 0x1; // (bit 27)

  PrintWord(0);
  if (isEnd) printf("SHDR End of spill data\n");
  else       printf("SHDR\n");
}
//______________________________________________________________________________
void TRawData::DecodeSTRL()
{
  // 0xD Spill trailer
  Bool_t isEnd = (fWord >> 27) & 0x1; // (bit 27)

  PrintWord(0);
  if (isEnd) printf("STRL End of spill data\n");
  else       printf("STRL\n");
}
//______________________________________________________________________________
void TRawData::DecodeSTAT()
{
  // 0xE Status
  PrintWord(0);
  printf("Status\n");
}
//______________________________________________________________________________
void TRawData::DecodeRESE()
{
  // 0xF Reserved
  PrintWord(0);
  printf("Reserved\n");
}
//______________________________________________________________________________
void TRawData::PrintWord(Int_t level, Bool_t space)
{
  TString nlevel;
  if (space) nlevel += " ";
  for (Int_t il = 0; il < level; il++) nlevel += "    ";
  printf("%6d: 0x%08X%s", fNWords, fWord, nlevel.Data());
}
//______________________________________________________________________________
void TRawData::DecodeOther()
{
  PrintWord(0, kFALSE);
  printf("\n");
}
