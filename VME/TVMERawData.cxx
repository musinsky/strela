// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    17 Mar 2014

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
  fEventMHDR(0),
  fFirstModule(kFALSE),
  fAllEvents(0)
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

  Printf("total size = %lu bytes", fNDataWords*sizeof(UInt_t));
  Printf("total words = %lu", fNDataWords);
  Printf("total spills = %d", fNSpills);
  Printf("total events = %d", fAllEvents);
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

      //    case kSTAT:
      //      DecodeSTAT();
      //      return;
      //    case kRESE:
      //      DecodeRESE();
      //      return;


      //    case kTHDR:
      //      DecodeTHDR();
      //      return;
      //    case kTTRL:
      //      DecodeTTRL();
      //      return;
      //    case kTLD:
      //      DecodeTLD();
      //      return;
      //    case kTTR:
      //      DecodeTTR();
      //      return;
      //    case kTERR:
      //      DecodeTERR();
      //      return;


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
    if (TestBit(kSpillEnd)) Warning("DecodeSHDR", "SHDR_END after previous SHDR_END");
    CheckIntegrity(kSpillEnd, kTRUE, "SHDR_END");
    //SetBit(kSpillEnd, kTRUE);
  }
  else {
    if (TestBit(kSpillEnd)) Warning("DecodeSHDR", "SHDR after previous SHDR_END");
    if (TestBit(kSpill)) Warning("DecodeSHDR", "SHDR after previous SHDR");
    CheckIntegrity(kSpill, kTRUE, "SHDR");
    //SetBit(kSpill, kTRUE);
  }

  // need correct work even in the case SHDR without STRL
  fAllEvents += fNEvents;
  fNEvents = 0;

  //  if (!PrintDataWord(0)) return;
  if (end) printf("SHDR_END\n");
  else     printf("SHDR ns= %d\n", fNSpills);
}
//______________________________________________________________________________
void TVMERawData::DecodeSTRL()
{
  // 0xD Spill trailer
  Bool_t end = (fDataWord >> 27) & 0x1; // (bit 27)

  if (end) {
    if (!TestBit(kSpillEnd)) Warning("DecodeSTRL", "STRL_END after previous STRL_END");
    CheckIntegrity(kSpillEnd, kFALSE, "STRL_END");
    //SetBit(kSpillEnd, kFALSE);
  }
  else {
    if (!TestBit(kSpill)) Warning("DecodeSTRL", "STRL after previous STRL");
    CheckIntegrity(kSpill, kFALSE, "STRL");
    //SetBit(kSpill, kFALSE);
    fNSpills++; // don't count spill_END
  }

  //  if (!PrintDataWord(0)) return;
  if (end) printf("STRL_END\n");
  else     printf("STRL ns= %d\n", fNSpills-1);
}
//______________________________________________________________________________
void TVMERawData::DecodeEHDR()
{
  // 0xA Event header
  Int_t ev = fDataWord & 0xFFFFF; // (bits 0 - 19)

  if (!(TestBit(kSpill) || TestBit(kSpillEnd))) Warning("DecodeEHDR", "EHDR out of SHDR");
  if (TestBit(kEvent)) Warning("DecodeEHDR", "EHDR after previous EHDR");
  // TestOutType(kEvent, "EHDR");
  CheckIntegrity(kEvent, kTRUE, "EHDR");
  // TestEvent(1,   kTRUE, "EHDR");
  //SetBit(kEvent, kTRUE);
  if (fNEvents != ev) Warning("DecodeEHDR", "Event number mismatch %d != %d", fNEvents, ev);


  fFirstModule = kTRUE;

  //return;
  if (!PrintDataWord(1)) return;
  printf("EHDR ev: %d\n", ev);
}
//______________________________________________________________________________
void TVMERawData::DecodeETRL()
{
  // 0xB Event trailer
  Int_t wc = fDataWord & 0xFFFFFF; // (bits 0 - 23)

  if (!(TestBit(kSpill) || TestBit(kSpillEnd))) Warning("DecodeETRL", "ETRL out of SHDR");
  if (!TestBit(kEvent)) Warning("DecodeETRL", "ETRL after previous ETRL");
  // TestOutType(kEvent, "ETRL");
  CheckIntegrity(kEvent, kFALSE, "ETRL");
  // TestEvent(1,    kFALSE, "ETRL");
  //SetBit(kEvent, kFALSE);
  fNEvents++;

  //return;
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

  if (fFirstModule) fFirstModule = kFALSE;
  else if (ev != fEventMHDR) Printf("!!!!!!!!!!!!!!!!!!!!!!!!!");
  fEventMHDR = ev;

  if (!(TestBit(kSpill) || TestBit(kSpillEnd))) Warning("DecodeMHDR", "MHDR out of SHDR");
  if (!TestBit(kEvent)) Warning("DecodeMHDR", "MHDR out of EHDR");
  ///////  TestEvent(0,   kFALSE, "MHDR");
  if (TestBit(kModule)) Warning("DecodeMHDR", "MHDR after previous MHDR");
  // TestOutType(kModule, "MHDR");
  CheckIntegrity(kModule, kTRUE, "MHDR");
  // TestModule(1,   kTRUE, "MHDR");
  //SetBit(kModule, kTRUE);
  if (fNEvents < (1 << 16)) { // max. event number for MHDR is 65536 (bits 0  - 15)
    if (fNEvents != ev) Warning("DecodeMHDR", "Event number mismatch %d != %d", fNEvents, ev);
    //    if (fNEvents != ev) Printf("Event number mismatch %d != %d", fNEvents, ev);
    //    else                Printf("Event number OK = %d", fNEvents);
  }

  //  return;
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

  if (!(TestBit(kSpill) || TestBit(kSpillEnd))) Warning("DecodeMTRL", "MTRL out of SHDR");
  if (!TestBit(kEvent)) Warning("DecodeMTRL", "MTRL out of EHDR");
  ////////////  TestModule(0,   kFALSE, "MTRL");
  if (!TestBit(kModule)) Warning("DecodeMTRL", "MTRL after previous MTRL");
  // TestOutType(kModule, "MTRL");
  CheckIntegrity(kModule, kFALSE, "MTRL");
  // TestModule(1,   kFALSE, "MTRL");
  //SetBit(kModule, kFALSE);
  if (er != 0xF) Warning("DecodeMTRL", "module error 0x%X", er);

  if (!PrintDataWord(2)) return;
  printf("MTRL wc: %d, er: 0x%X, crc: 0x%X\n", wc, er, crc);
}


//______________________________________________________________________________
void TVMERawData::CheckIntegrity(ETypeStatus type, Bool_t status, const char *where)
{
  // first check: header after trailer or trailer after header

  if (TestBit(type) == status) Warning("CheckIntegrity", "%s after previous %s", where, where);
  SetBit(type, status);
  //if (type != kEvent && status != 0) SetBit(type, status);

  CheckIntegrity2(type, where);
}
//______________________________________________________________________________
void TVMERawData::CheckIntegrity2(ETypeStatus type, const char *where)
{
  // second check: module inside event and simultaneously event inside spill

  if (type == kModule) {            // module inside event
    if (TestBit(kEvent) == kFALSE)
      Warning("CheckIntegrity", "%s out of EHDR", where);
    CheckIntegrity2(kEvent, where); // and simultaneously event inside spill
  }
  else if (type == kEvent) {        // event inside spill
    if ((TestBit(kSpill) == kFALSE) && (TestBit(kSpillEnd) == kFALSE))
      Warning("CheckIntegrity", "%s out of SHDR", where);
  }
}
//______________________________________________________________________________
void TVMERawData::DecodeOther()
{
  return;
  if (!PrintDataWord(0)) return;
  printf("other\n");
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





