// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    26 Feb 2015

#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>

#include "TVMERawData.h"
#include "TVME.h"
#include "TVMEEvent.h"
#include "TVirtualModule.h"

ClassImp(TVMERawData)

//______________________________________________________________________________
TVMERawData::TVMERawData()
: TObject(),
  fDataWord(0),
  fDataType(0),
  fNDataWords(0),
  fNSpills(0),
  fNEvents(0),
  fEventEHDR(0),
  fEventMHDR(0),
  fModuleId(0),
  fPrintType(0),
  fTree(0),
  fVMEEvent(0),
  fModule(0),
  fTreeFileName()
{
  // Default constructor
  fPrintType = new TBits(16); // kPADD+1
}
//______________________________________________________________________________
TVMERawData::~TVMERawData()
{
  // Destructor
  SafeDelete(fPrintType);
}
//______________________________________________________________________________
void TVMERawData::Reset()
{
  fNDataWords = fNSpills = fNEvents = 0;
  ResetBit(kSpill);
  ResetBit(kSpillEnd);
}
//______________________________________________________________________________
void TVMERawData::MakeTree(const char *fname)
{
  if (!gVME) {
    Error("MakeTree", "gVME not initialized");
    return;
  }

  TString treeFileName = fTreeFileName;
  if (fTreeFileName == "root") {
    // replace file extension by ".root" and save in current dir
    treeFileName = gSystem->BaseName(fname);
    if (treeFileName.Last('.') != -1) treeFileName.Remove(treeFileName.Last('.'));
    treeFileName += ".root";
  }
  else if (fTreeFileName.IsWhitespace())
    treeFileName = "vme_data.root";

  new TFile(treeFileName.Data(), "RECREATE");
  fTree = new TTree("pp", gVME->GetName());
  fTree->SetAutoSave(1000000000); // autosave when 1 Gbyte written
  fVMEEvent = new TVMEEvent();
  fTree->Branch("VMEEvent", &fVMEEvent);
  //  fTree->Branch("VMEEvent", fVMEEvent->ClassName(), &fVMEEvent);
}
//______________________________________________________________________________
void TVMERawData::DecodeFile(const char *fname, Bool_t tree)
{
  FILE *file = fopen(fname, "rb");
  if (!file) {
    Error("DecodeFile", "file %s can not be opened", fname);
    return;
  }

  Reset();
  if (tree) MakeTree(fname);
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

  if (fTree) {
    TFile *tfile = fTree->GetCurrentFile();
    tfile->Write();
    Printf("\ntree file: %s", tfile->GetName());
    Printf("events = %llu", fTree->GetEntries());
    SafeDelete(fVMEEvent);
    SafeDelete(fTree);
    delete tfile;
  }
  fclose(file);

  Printf("\ndata file: %s", fname);
  Printf("size   = %lu bytes (%lu words)", fNDataWords*sizeof(UInt_t), fNDataWords);
  Printf("spills = %d", fNSpills);
  Printf("events = %d", fNEvents);
}
//______________________________________________________________________________
void TVMERawData::DecodeDataWord()
{
  fDataType = fDataWord >> 28; // (bits 28 - 31)

  switch (fDataType) {
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
    case kPADD:
      DecodePADD();
      return;
    default:
      DecodeData();
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

  if (!PrintDataType(0)) return;
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

  if (!PrintDataType(0)) return;
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

  // reset
  fEventMHDR = -1;
  SetBit(kWrongEvent, kFALSE);
  if (fTree) fVMEEvent->Clear();

  if (!PrintDataType(1)) return;
  printf("EHDR ev: %d\n", ev);
}
//______________________________________________________________________________
void TVMERawData::DecodeETRL()
{
  // 0xB Event trailer
  Int_t wc = fDataWord & 0xFFFFFF; // (bits 0 - 23)

  CheckIntegrity(kEvent, kFALSE, "ETRL");
  if (TestBit(kWrongEvent)) {
    Warning("DecodeETRL", "wrong event = %d", fNEvents + fEventEHDR);
    if (fTree) fVMEEvent->Clear();
  }

  if (fTree && !TestBit(kSpillEnd)) {
    fVMEEvent->SetEvent(fEventEHDR);
    fTree->Fill();
  }

  if (!PrintDataType(1)) return;
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
  fModuleId = id;

  if (gVME) {
    fModule = gVME->GetModule(ga); // suppose that ga is < 20
    if (fModule && (fModule->GetId() != id)) {
      Error("DecodeMHDR", "incorrect module ID %d != %d", id, fModule->GetId());
      fModule = 0;
    }
  }

  if (!PrintDataType(2)) return;
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

  if (!PrintDataType(2)) return;
  printf("MTRL wc: %d, er: 0x%X, crc: 0x%X\n", wc, er, crc);
}
//______________________________________________________________________________
void TVMERawData::DecodeSTAT()
{
  // 0xE Status

  if (!PrintDataType(0)) return;
  printf("Status\n");
}
//______________________________________________________________________________
void TVMERawData::DecodePADD()
{
  // 0xF Padding

  if (!PrintDataType(0)) return;
  printf("Padding\n");
}
//______________________________________________________________________________
void TVMERawData::DecodeData()
{
  // 0x0 - 0x7 Data specifying by module ID

  CheckIntegrity2(kData, "Data");

  switch (fModuleId) {
    case kTDC64V:
      DecodeDataTDC();
      return;
    case kTQDC16:
      DecodeDataTQDC();
      return;
      //    case kTDC96:
      //      DecodeDataTDC();
      //      return;
      //    case kTRIG:
      //      DecodeDataTTCM();
      //      return;
    default:
      break;
  }

  if (!PrintDataType(3)) return;
  printf("DATA not specified\n");
}
//______________________________________________________________________________
void TVMERawData::DecodeDataTDC()
{
  // 0x2 - 0x6 only for TDC Data format

  switch (fDataType) {
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
      break;
  }

  if (!PrintDataType(3)) return;
  printf("DATA TDC not specified\n");
}
//______________________________________________________________________________
void TVMERawData::DecodeTHDR()
{
  // 0x2 TDC header (nothing interesting)
  Int_t ts = fDataWord & 0xFFF;         // (bits 0  - 11)
  Int_t ev = (fDataWord >> 12) & 0xFFF; // (bits 12 - 23)
  Int_t id = (fDataWord >> 24) & 0xF;   // (bits 24 - 27)

  if (!PrintDataType(3)) return;
  printf("THDR ts: %d, ev: %d, id: %2d\n", ts, ev, id);
}
//______________________________________________________________________________
void TVMERawData::DecodeTTRL()
{
  // 0x3 TDC trailer (nothing interesting)
  Int_t wc = fDataWord & 0xFFF;         // (bits 0  - 11)
  Int_t ev = (fDataWord >> 12) & 0xFFF; // (bits 12 - 23)
  Int_t id = (fDataWord >> 24) & 0xF;   // (bits 24 - 27)

  if (!PrintDataType(3)) return;
  printf("TTRL wc: %d, ev: %d, id: %2d\n", wc, ev, id);
}
//______________________________________________________________________________
void TVMERawData::DecodeTLD()
{
  // 0x4 TDC leading (Single edge measurements)
  Int_t tm = fDataWord & 0x7FFFF;      // (bits 0  - 18)
  Int_t ch = (fDataWord >> 19) & 0x1F; // (bits 19 - 23)
  Int_t id = (fDataWord >> 24) & 0xF;  // (bits 24 - 27)

  // correct only for normal resolution, 19 bits (100ps)
  // see also daqlib/hptdc/hptdc.cpp (by Ilja Slepnev)
  //
  // for very high resolution, 21 bits (25ps)
  // tm = ((fDataWord & 0x7FFFF) << 2) | ((fDataWord >> 19) & 0x3);
  // and ! different channel decoding !

  if (fModule && fTree)
    fVMEEvent->AddTDCHitCheck(fModule->GetFirstChannel() + fModule->MapChannel(id, ch), tm, kTRUE);

  if (!PrintDataType(3)) return;
  printf("TLD tm: %6d, ch: %2d, id: %2d\n", tm, ch, id);
}
//______________________________________________________________________________
void TVMERawData::DecodeTTR()
{
  // 0x5 TDC trailing (Single edge measurements)
  Int_t tm = fDataWord & 0x7FFFF;      // (bits 0  - 18)
  Int_t ch = (fDataWord >> 19) & 0x1F; // (bits 19 - 23)
  Int_t id = (fDataWord >> 24) & 0xF;  // (bits 24 - 27)

  // normal resolution, same remarks as in DecodeTLD

  if (fModule && fTree)
    fVMEEvent->AddTDCHitCheck(fModule->GetFirstChannel() + fModule->MapChannel(id, ch), tm, kFALSE);

  if (!PrintDataType(3)) return;
  printf("TTR tm: %6d, ch: %2d, id: %2d\n", tm, ch, id);
}
//______________________________________________________________________________
void TVMERawData::DecodeTERR()
{
  // 0x6 TDC error
  Int_t flag = fDataWord & 0x7FFF; // (bits 0  - 14)

  Warning("DecodeTERR", "TDC error flags: 0x%X", flag);
}
//______________________________________________________________________________
void TVMERawData::DecodeDataTQDC()
{
  // 0x0 - 0x6 only for TQDC Data format

  switch (fDataType) {
    case kTQCHI:
      DecodeTQCHI();
      return;
    case kTQCLO:
      DecodeTQCLO();
      return;
    case kTQHDR:
      DecodeTQHDR();
      return;
    case kTQTRL:
      DecodeTQTRL();
      return;
    case kTQDC4:
      DecodeTQDC4();
      return;
    case kTQDC5:
      DecodeTQDC5();
      return;
    case kTQERR:
      DecodeTQERR();
      return;
    default:
      break;
  }

  if (!PrintDataType(3)) return;
  printf("DATA TQDC not specified\n");
}
//______________________________________________________________________________
void TVMERawData::DecodeTQCHI()
{
  // 0x0 TQDC input counters high bits 31:16 (nothing interesting)
  Int_t chi = fDataWord & 0xFFFF;       // (bits 0  - 15)
  Int_t ch  = (fDataWord >> 19) & 0x1F; // (bits 19 - 23)

  // data (counters/rate) only between SHDR_END and STRL_END

  if (!PrintDataType(3)) return;
  printf("TQCHI chi: %6d, ch: %d\n", chi, ch);
}
//______________________________________________________________________________
void TVMERawData::DecodeTQCLO()
{
  // 0x1 TQDC input counters low bits 15:0 (nothing interesting)
  Int_t clo = fDataWord & 0xFFFF;       // (bits 0  - 15)
  Int_t ch  = (fDataWord >> 19) & 0x1F; // (bits 19 - 23)

  if (!PrintDataType(3)) return;
  printf("TQCLO clo: %6d, ch: %d\n", clo, ch);
}
//______________________________________________________________________________
void TVMERawData::DecodeTQHDR()
{
  // 0x2 TQDC header (nothing interesting)
  Int_t ts = fDataWord & 0xFFF;         // (bits 0  - 11)
  Int_t ev = (fDataWord >> 12) & 0xFFF; // (bits 12 - 23)

  if (!PrintDataType(3)) return;
  printf("TQHDR ts: %d, ev: %d\n", ts, ev);
}
//______________________________________________________________________________
void TVMERawData::DecodeTQTRL()
{
  // 0x3 TQDC trailer (nothing interesting)
  Int_t wc = fDataWord & 0xFFF;         // (bits 0  - 11)
  Int_t ev = (fDataWord >> 12) & 0xFFF; // (bits 12 - 23)

  if (!PrintDataType(3)) return;
  printf("TQTRL wc: %d, ev: %d\n", wc, ev);
}
//______________________________________________________________________________
void TVMERawData::DecodeTQDC4()
{
  // 0x4 TQDC data (ADC timestamp or TDC data)
  Int_t ch   = (fDataWord >> 19) & 0x1F; // (bits 19 - 23)
  Int_t mode = (fDataWord >> 26) & 0x3;  // (bits 26 - 27)

  if (mode != 0) { // ADC timestamp
    Int_t ts  = fDataWord & 0xFFFF;      // (bits 0  - 15)
    Int_t wts = (fDataWord >> 16) & 0x7; // (bits 16 - 18)

    if (wts == 0) {
      if (!PrintDataType(3)) return;
      printf("TQDC4 ts(TRIG): %6d, ch: %2d, wts: %d, mode: %d\n", ts, ch, wts, mode);
    }
    else if (wts == 1) {
      if (!PrintDataType(3)) return;
      printf("TQDC4 ts(ADC ): %6d, ch: %2d, wts: %d, mode: %d\n", ts, ch, wts, mode);
    }
    else {
      if (!PrintDataType(3)) return;
      printf("TQDC4 ts(!!!!): %6d, ch: %2d, wts: %d, mode: %d\n", ts, ch, wts, mode);
    }
  }
  else {           // TDC data
    Int_t tm = fDataWord & 0x7FFFF;      // (bits 0  - 18)

    // correct only for normal resolution, 19 bits (100ps)
    //
    // for very high resolution, 21 bits (25ps)
    // tm = ((fDataWord & 0x7FFFF) << 2) | ((fDataWord >> 24) & 0x3);

    if (!PrintDataType(3)) return;
    printf("TQDC4 tm: %6d, ch: %2d, mode: %d\n", tm, ch, mode);
  }
}
//______________________________________________________________________________
void TVMERawData::DecodeTQDC5()
{
  // 0x5 TQDC data (TDC or ADC data)
  Int_t ch   = (fDataWord >> 19) & 0x1F; // (bits 19 - 23)
  Int_t mode = (fDataWord >> 26) & 0x3;  // (bits 26 - 27)

  if (mode == 0) {      // TDC data
    Int_t tm = fDataWord & 0x7FFFF;      // (bits 0  - 18)

    // normal resolution, same remarks as in DecodeTQDC4 (mode == 0)

    if (!PrintDataType(3)) return;
    printf("TQDC5 tm: %6d, ch: %2d, mode: %d\n", tm, ch, mode);
  }
  else if (mode == 2) { // ADC sampling
    const Int_t kADCBits = 14; // 0x3FFF    (bits 0  - 13)
    Int_t sample = (fDataWord & ((1 << kADCBits) - 1)) - (1 << (kADCBits - 1));
    // same as     (fDataWord & 0x3FFF)                - (1 << (kADCBits - 1));

    // ADC is sampling at 80 MHz with resolution of 14 bits (one sample 12.5ns)
    // (old module revision 100 MHz and 10 bits)

    if (!PrintDataType(3)) return;
    printf("TQDC5   sample: %6d, ch: %2d, mode: %d\n", sample, ch, mode);
  }
  else {
    if (!PrintDataType(3)) return;
    printf("TQDC5 ch: %2d, unsupported mode: %d\n", ch, mode);
  }
}
//______________________________________________________________________________
void TVMERawData::DecodeTQERR()
{
  // 0x6 TQDC error
  Int_t flag = fDataWord & 0x7FFF; // (bits 0  - 14)

  Warning("DecodeTQERR", "TQDC error flags: 0x%X", flag);
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
    if (TestBit(kModule) == kFALSE) {
      Warning("CheckIntegrity", "%s out of MHDR", where);
      SetBit(kWrongEvent, kTRUE);
    }
    CheckIntegrity2(kModule, where); // recursive check module (and event)
  }
  else if (type == kModule) {        // module inside event
    if (TestBit(kEvent) == kFALSE) {
      Warning("CheckIntegrity", "%s out of EHDR", where);
      SetBit(kWrongEvent, kTRUE);
    }
    CheckIntegrity2(kEvent, where);  // recursive check event
  }
  else if (type == kEvent) {         // event inside spill
    if ((TestBit(kSpill) == kFALSE) && (TestBit(kSpillEnd) == kFALSE))
      Warning("CheckIntegrity", "%s out of SHDR", where);
  }
}
//______________________________________________________________________________
Bool_t TVMERawData::PrintDataType(Int_t nlevel) const
{
  /*
  Int_t pt = 15360; // 0011110000000000 (bits 10-13) => events + spills
  Int_t pt = 0xFFFF // 1111111111111111 (bits 00-15) => all
  fPrintType->Set(16, &pt);

  fPrintType->ResetAllBits();
  fPrintType->SetBitNumber(TVMERawData::kSHDR);
  fPrintType->SetBitNumber(TVMERawData::kSTRL);

  // binary representation of fDataWord
  for (UInt_t ib = 1 << 31; ib > 0; ib = ib/2) (fDataWord & ib) ? printf("1") : printf("0");
   */

  if (!fPrintType->TestBitNumber(fDataType)) return kFALSE;

  TString level = " ";
  for (Int_t i = 0; i < nlevel; i++) level += "    ";
  printf("%12lu: [0x%08X]%s", fNDataWords, fDataWord, level.Data());
  return kTRUE;
}
