// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Mar 2014

//#include <TFile.h>

#include "TVMERawData.h"
#include "TVME.h"

ClassImp(TVMERawData)

//______________________________________________________________________________
TVMERawData::TVMERawData()
: TObject(),
  fDataWord(0),
  fNDataWords(0),
  fNSpills(0),
  fNEvents(0),
  fEventEHDR(0),
  fEventMHDR(0)
{
  // Default constructor
}
//______________________________________________________________________________
TVMERawData::~TVMERawData()
{
  // Destructor
}
//______________________________________________________________________________
void TVMERawData::ReadFile(const char *fname)
{
  FILE *file = fopen(fname, "rb");
  if (!file) {
    Error("ReadFile", "file %s can not be opened", fname);
    return;
  }

  const size_t bufSize = 4096; // 4096 is blocksize in bytes (blockdev --getbsz /dev/sda1)
  UInt_t buffer[bufSize];      // data in buffer is 4 bytes (32 bits)
  size_t nread;

  do {
    nread = fread(buffer, sizeof(UInt_t), bufSize, file);
    for (size_t i = 0; i < nread; i++) {
      fDataWord = buffer[i];
      DecodeDataWord();
      fNDataWords++;
    }
  } while (nread == bufSize);

  fclose(file);

  Printf("\nfile: %s", fname);
  Printf("size   = %lu bytes", fNDataWords*sizeof(UInt_t));
  Printf("words  = %lu", fNDataWords);
  Printf("spills = %d", fNSpills);
  Printf("events = %d", fNEvents);
}
//______________________________________________________________________________
void TVMERawData::DecodeDataWord()
{
  UInt_t dataType = fDataWord >> 28; // (bits 28 - 31)

  switch (dataType) {
    case kSHDR:
      DecodeSHDR();
      return;
    case kSTRL:
      DecodeSTRL();
      return;
    case kEHDR:
      DecodeEHDR();
      return;
    case kETRL:
      DecodeETRL();
      return;
    case kMHDR:
      DecodeMHDR();
      return;
    case kMTRL:
      DecodeMTRL();
      return;
    case kSTAT:
      DecodeSTAT();
      return;
    case kRESE:
      DecodeRESE();
      return;
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
    default:
      DecodeOther();
      return;
  }
}
//______________________________________________________________________________
void TVMERawData::DecodeSHDR()
{
  // 0xC Spill header
  Bool_t end = (fDataWord >> 27) & 0x1; // (bit 27)

  if (end) {
    if (TestBit(kSpill)) Warning("DecodeSHDR", "SHDR_END after previous SHDR");
    CheckIntegrity(kSpillEnd, kTRUE, "SHDR_END");

    // exclude spill (and event) from end of spill data
    fNSpills++;
    // don't sum in DecodeSTRL, need correct count also in the case SHDR without STRL
    fNEvents += (fEventEHDR + 1);
  }
  else {
    if (TestBit(kSpillEnd)) Warning("DecodeSHDR", "SHDR after previous SHDR_END");
    CheckIntegrity(kSpill, kTRUE, "SHDR");
  }

  fEventEHDR = -1; // reset

  //if (!PrintDataWord(0)) return;
  if (end) printf("SHDR_END\n");
  else     printf("SHDR ns= %d\n", fNSpills);
}
//______________________________________________________________________________
void TVMERawData::DecodeSTRL()
{
  // 0xD Spill trailer
  Bool_t end = (fDataWord >> 27) & 0x1; // (bit 27)

  if (end) CheckIntegrity(kSpillEnd, kFALSE, "STRL_END");
  else CheckIntegrity(kSpill, kFALSE, "STRL");

  //if (!PrintDataWord(0)) return;
  if (end) printf("STRL_END\n");
  else     printf("STRL ns= %d\n", fNSpills);
}
//______________________________________________________________________________
void TVMERawData::DecodeEHDR()
{
  // 0xA Event header
  Int_t ev = fDataWord & 0xFFFFF; // (bits 0 - 19)

  CheckIntegrity(kEvent, kTRUE, "EHDR");
  if ((ev - fEventEHDR) != 1) Warning("DecodeEHDR", "discontiguous event number %d after %d", ev, fEventEHDR);
  fEventEHDR = ev;

  fEventMHDR = -1; // reset

  if (!PrintDataWord(1)) return;
  printf("EHDR ev: %d\n", ev);
}
//______________________________________________________________________________
void TVMERawData::DecodeETRL()
{
  // 0xB Event trailer
  Int_t wc = fDataWord & 0xFFFFFF; // (bits 0 - 23)

  CheckIntegrity(kEvent, kFALSE, "ETRL");

  if (!PrintDataWord(1)) return;
  printf("ETRL wc: %d\n", wc);
}
//______________________________________________________________________________
void TVMERawData::DecodeMHDR()
{
  // 0x8 Module header
  Int_t ev = fDataWord & 0xFFFF;       // (bits 0  - 15)
  Int_t id = (fDataWord >> 16) & 0x7F; // (bits 16 - 22)
  Int_t ga = (fDataWord >> 23) & 0x1F; // (bits 23 - 27)

  CheckIntegrity(kModule, kTRUE, "MHDR");
  // in some cases fEventMHDR != fEventEHDR and not starting from 0
  if (fEventMHDR == -1) fEventMHDR = ev;
  else if (ev != fEventMHDR) Warning("DecodeMHDR", "event number mismatch %d != %d", ev, fEventMHDR);
  fEventMHDR = ev;

  if (!PrintDataWord(2)) return;
  printf("MHDR ev: %d, id: %2d, ga: %2d\n", ev, id, ga);
}
//______________________________________________________________________________
void TVMERawData::DecodeMTRL()
{
  // 0x9 Module trailer
  Int_t wc  = fDataWord & 0xFFFF;       // (bits 0  - 15)
  Int_t er  = (fDataWord >> 16) & 0xF;  // (bits 16 - 19)
  Int_t crc = (fDataWord >> 20) & 0xFF; // (bits 20 - 27)

  CheckIntegrity(kModule, kFALSE, "MTRL");
  if (er != 0xF) Warning("DecodeMTRL", "module error 0x%X", er);

  if (!PrintDataWord(2)) return;
  printf("MTRL wc: %d, er: 0x%X, crc: 0x%X\n", wc, er, crc);
}
//______________________________________________________________________________
void TVMERawData::DecodeSTAT()
{
  // 0xE Status

  if (!PrintDataWord(0)) return;
  printf("STAT\n");
}
//______________________________________________________________________________
void TVMERawData::DecodeRESE()
{
  // 0xF Reserved

  if (!PrintDataWord(0)) return;
  printf("RESE\n");
}
//______________________________________________________________________________
void TVMERawData::DecodeOther()
{
  if (!PrintDataWord(0)) return;
  printf("other\n");
}
//______________________________________________________________________________
void TVMERawData::DecodeTHDR()
{
  // 0x2 TDC header
  Int_t ts = fDataWord & 0xFFF;         // (bits 0  - 11)
  Int_t ev = (fDataWord >> 12) & 0xFFF; // (bits 12 - 23)
  Int_t id = (fDataWord >> 24) & 0xF;   // (bits 24 - 27)

  // obsolete (it will not be stored)

  if (!PrintDataWord(3)) return;
  printf("THDR ts: %d, ev: %d, id: %2d\n", ts, ev, id);
}
//______________________________________________________________________________
void TVMERawData::DecodeTTRL()
{
  // 0x3 TDC trailer
  Int_t wc = fDataWord & 0xFFF;         // (bits 0  - 11)
  Int_t ev = (fDataWord >> 12) & 0xFFF; // (bits 12 - 23)
  Int_t id = (fDataWord >> 24) & 0xF;   // (bits 24 - 27)

  // obsolete (it will not be stored)

  if (!PrintDataWord(3)) return;
  printf("TTRL wc: %d, ev: %d, id: %2d\n", wc, ev, id);
}
//______________________________________________________________________________
void TVMERawData::DecodeTLD()
{
  // 0x4 TDC leading (Single edge measurements)
  Int_t tm = fDataWord & 0x7FFFF;      // (bits 0  - 18)
  Int_t ch = (fDataWord >> 19) & 0x1F; // (bits 19 - 23)
  Int_t id = (fDataWord >> 24) & 0xF;  // (bits 24 - 27)

  CheckIntegrity2(kData, "TLD");

  if (!PrintDataWord(4)) return;
  printf("TLD tm: %6d, ch: %2d, id: %2d\n", tm, ch, id);
}
//______________________________________________________________________________
void TVMERawData::DecodeTTR()
{
  // 0x5 TDC trailing (Single edge measurements)
  Int_t tm = fDataWord & 0x7FFFF;      // (bits 0  - 18)
  Int_t ch = (fDataWord >> 19) & 0x1F; // (bits 19 - 23)
  Int_t id = (fDataWord >> 24) & 0xF;  // (bits 24 - 27)

  CheckIntegrity2(kData, "TTR");

  if (!PrintDataWord(4)) return;
  printf("TTR tm: %6d, ch: %2d, id: %2d\n", tm, ch, id);
}
//______________________________________________________________________________
void TVMERawData::DecodeTERR()
{
  // 0x6 TDC error
  Int_t flag = fDataWord & 0x7FFF; // (bits 0  - 14)

  CheckIntegrity2(kData, "TERR");

  Warning("DecodeTERR", "TDC error flags: 0x%X", flag);
}
//______________________________________________________________________________
void TVMERawData::CheckIntegrity(ETypeStatus type, Bool_t status, const char *where)
{
  // sequencing of data
  // header after trailer or trailer after header

  if (TestBit(type) == status) Warning("CheckIntegrity", "%s after previous %s", where, where);
  SetBit(type, status);

  CheckIntegrity2(type, where);
}
//______________________________________________________________________________
void TVMERawData::CheckIntegrity2(ETypeStatus type, const char *where)
{
  // embedding of data
  // data inside module and simultaneously module inside event and event inside spill
  // recursive calling (simultaneously checking) probably is not necessary

  if (type == kData) {               // data inside module
    if (TestBit(kModule) == kFALSE)
      Warning("CheckIntegrity", "%s out of MHDR", where);
    CheckIntegrity2(kModule, where); // recursive check module (and event)
  }
  else if (type == kModule) {        // module inside event
    if (TestBit(kEvent) == kFALSE)
      Warning("CheckIntegrity", "%s out of EHDR", where);
    CheckIntegrity2(kEvent, where);  // recursive check event
  }
  else if (type == kEvent) {         // event inside spill
    if ((TestBit(kSpill) == kFALSE) && (TestBit(kSpillEnd) == kFALSE))
      Warning("CheckIntegrity", "%s out of SHDR", where);
  }
}
//______________________________________________________________________________
Bool_t TVMERawData::PrintDataWord(Int_t nlevel) const
{
  return kFALSE;
  TString level = " ";
  for (Int_t i = 0; i < nlevel; i++) level += "    ";
  printf("%12lu: [0x%08X]%s", fNDataWords, fDataWord, level.Data());
  return kTRUE;
}
