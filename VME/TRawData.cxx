// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    05 Mar 2014

#include <fcntl.h>
#include <TError.h>
#include <TTree.h>
#include <TFile.h>

#include "TRawData.h"
#include "TVME.h"
#include "TVirtualModule.h"
#include "TGemEvent.h"


#include <iostream>
#include <cstdio>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <inttypes.h>

// ROZDELIT 0-7 zavisdai od moula !!!!
// 8-F univerzalne !!!!!!!!!!!!!!!!


// toto je ine pre TDC TQDC a pod.
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
  fNSpills(0),
  fNEvent(0),
  fEvent(-1),
  fTdcId(-1),
  fModule(0),
  fTree(0),
  fGemEvent(0)
{
  // Default constructor
  // check na gvme a konec
}
//______________________________________________________________________________
TRawData::~TRawData()
{
  // Destructor
}
//______________________________________________________________________________
void TRawData::ParseFile(const char *fname)
{
  //  http://tecadmin.net/flush-memory-cache-on-linux-server
  //  // echo 3 > /proc/sys/vm/drop_caches
  //  // echo 3 | sudo tee /proc/sys/vm/drop_caches
  //  // sync; echo 3 > /proc/sys/vm/drop_caches      !!!!!!!!!!!!
  //
  //
  //  const Int_t BUFLEN = 4096*1;  // 4096 is blocksize in bytes (blockdev --getbsz /dev/sda1)
  //  UInt_t buf[BUFLEN];
  //  FILE *f = fopen(fname, "rb");
  //  size_t nread = 1;
  //  Int_t count = 0;
  //  fseek(f , 0 , SEEK_END);
  //  UInt_t fsize = ftell(f)/4;
  //  rewind(f);
  //  UInt_t position = 0;
  //  UInt_t position2 = 0;
  //
  //  while (position < fsize) {
  ////    nread = fread(&buf, sizeof(UInt_t), BUFLEN, f);
  //    //nread = fread(&fWord, sizeof(fWord), 1, f);
  //    nread = fread(&buf, BUFLEN, 1, f);
  //    Int_t max = BUFLEN;
  //    if (BUFLEN > (fsize-position)) max = fsize-position;
  //
  //    for (size_t i=0; i<max; i++) {
  ////      uint32_t d = buf[i];
  ////      uint32_t code = d >> 28;
  //      Printf("%6d   0x%08X", position, buf[i]);
  ////      if (position >= fsize) break;
  //      position++;
  //    }
  //////    Printf("%ld", nread);
  ////    position2 += position;
  //    Printf("%6d   0x%08X", max-1, buf[max-1]);
  //  }
  //  Printf("%", position);
  //  return;

  const UInt_t kBufSize = 4096*1; // 4096 is blocksize in bytes (blockdev --getbsz /dev/sda1)
  UInt_t dataWord[kBufSize];      // data is 4 bytes (32 bits). Intel byte order is used
  // asi lepsie dataWord na buffer
  // a potom data = buffer[i];
  fFile = fopen(fname, "rb");
  if (!fFile) {
    Error("ParseFile", "file %s can not be opened", fname);
    return;
  }


  fseek(fFile , 0 , SEEK_END);
  UInt_t fsize = ftell(fFile)/4;
  Printf("size = %d", fsize);
  rewind(fFile);

  struct stat file_stats;
  // TUnixSystem.cxx; rootx.cxx -> static int ReadUtmp()
  stat(fname, &file_stats);
  size_t nread, size;   // je unsigned size_t is 8 bytes (ULong_t)
  size = file_stats.st_size;
  printf("b  %9jd \n", size);
  Printf("size = %zu", size/4);

  // vector<int> numbers(512);
  // numbers.size() = 512
  // if(fread(&numbers[0], sizeof(int),numbers.size(),fd)!=numbers.size())

  //  TUnixSystem::CopyFile
  //  const int bufsize = 1024;
  //  char buf[bufsize];
  //  int ret = 0;
  //  while (!ret && !feof(from)) {
  //     size_t numread    = fread (buf, sizeof(char), bufsize, from);
  //     size_t numwritten = fwrite(buf, sizeof(char), numread, to);
  //     if (numread != numwritten)
  //        ret = -3;
  //  }

  /* OKOKOK
  const size_t bufSize = 4096; // 4096 is blocksize in bytes (blockdev --getbsz /dev/sda1)
  UInt_t buffer[bufSize];      // data in buffer is 4 bytes (32 bits)

  do {
    nread = fread(buffer, sizeof(UInt_t), bufSize, fFile);
    for (size_t i = 0; i < nread; i++) {
      Printf("%03zu) %d", i, buffer[i]>>28);
      fWord = buffer[i]; // fWord na fData
    }
  } while (nread == bufSize);
   */
  // size_t to unsigned long

  fFile = fopen(fname, "rb");
  if (!fFile) {
    Error("ParseFile", "file %s can not be opened", fname);
    return;
  }




  do {
    nread = fread(dataWord, sizeof(UInt_t), kBufSize, fFile);

    //nread = fread(dataWord, sizeof(UInt_t), kBufSize, fFile);

    //nread = fread(&dataWord[0], sizeof(UInt_t), kBufSize, fFile);
    //nread = fread(&dataWord, sizeof(UInt_t), kBufSize, fFile);
    for (size_t i=0;i<nread;i++)
      ;
//      Printf("%03zu) %d", i, dataWord[i]>>28); /// ale size_t je fiyr menise ako kBufSize ktore ke int
//    Printf("== %zu", nread);
  } while (nread==kBufSize);

  //nread = fread(&fWord, sizeof(fWord), 1, f);
  //nread = fread(&buf, BUFLEN, 1, f);

  //  UInt_t position = 0;
  //  UInt_t position2 = 0;
  //
  nread = fread(&fWord, sizeof(fWord), 1, fFile);
  Printf("%zu", nread);
  nread = fread(&fWord, sizeof(fWord), 10, fFile);
  Printf("%zu", nread);


  return;
  //  TFile *file = new TFile("run100.root", "RECREATE");
  //  fTree = new TTree("pp", "nove");
  //  fTree->SetAutoSave(1000000000); // autosave when 1 Gbyte written
  //  fGemEvent = new TGemEvent();
  //  TObject *address = fGemEvent;
  //  //fTree->Branch("event", fGemEvent->ClassName(), &fGemEvent);
  //  fTree->Branch("event", fGemEvent->ClassName(), &address);

  fNWords  = 0;
  fNSpills = 0;
  fNEvent  = 0;
  while (fread(&fWord, sizeof(fWord), 1, fFile) == 1) {
    FindType();
    fNWords++;
    if (fNWords>100) break;
  }
  fclose(fFile);

  //  fTree->Write();
  //  delete file;
}
//______________________________________________________________________________
void TRawData::FindType2()
{
  UInt_t type = fWord >> 28; // (bits 28 - 31)
  switch (type) {
    case kEHDR:
      DecodeEHDR();    // pozor nie ehdr ak je spill s ENDOM !!!!!!!!!
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
    case kTLD:
      DecodeTLD();
      return;
    case kTTR:
      DecodeTTR();
      return;
  }

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
      DecodeStatus();
      return;
    case kRESE:
      DecodeReserved();
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
    //    PrintWord(4);
    //    printf("noTDC\n");
    return;
  }

  // 0x4 TDC leading
  Int_t tdc = fWord & 0x7FFFF;      // (bits 0  - 18)
  Int_t ch  = (fWord >> 19) & 0x1F; // (bits 19 - 23)
  Int_t id  = (fWord >> 24) & 0xF;  // (bits 24 - 27)

  //if (id != fTdcId) Error("DecodeTLD", "tdc_id mismatch %d != %d", id, fTdcId);

  //  PrintWord(4);
  //  printf("TLD tdc: %4d, ch: %2d, id: %2d (nadc: %02d)\n", tdc, ch, id, fModule->MapChannel(id, ch));
  fModule->MultiHitAdd(fModule->MapChannel(id, ch), tdc, kTRUE);
}
//______________________________________________________________________________
void TRawData::DecodeTTR()
{
  if (!fModule) {
    //    PrintWord(4);
    //    printf("noTDC\n");
    return;
  }

  // 0x5 TDC trailing
  Int_t tdc = fWord & 0x7FFFF;      // (bits 0  - 18)
  Int_t ch  = (fWord >> 19) & 0x1F; // (bits 19 - 23)
  Int_t id  = (fWord >> 24) & 0xF;  // (bits 24 - 27)

  //if (id != fTdcId) Error("DecodeTTR", "tdc_id mismatch %d != %d", id, fTdcId);

  //  PrintWord(4);
  //  printf("TTR tdc: %4d, ch: %2d, id: %2d (nadc: %02d)\n", tdc, ch, id, fModule->MapChannel(id, ch));
  fModule->MultiHitAdd(fModule->MapChannel(id, ch), tdc, kFALSE);
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
  // ev nepotrebny
  Int_t id = (fWord >> 16) & 0x7F; // (bits 16 - 22)
  Int_t ga = (fWord >> 23) & 0x1F; // (bits 23 - 27)



  // viem dekodovat len TDC moduly. t.j. id == 0x10 (16), 0x04, 0x05;
  // rozlsovci smDaboooooooooooooo TQQDC

  // nastavit fModule=0 pre zly pripad, nech nedokoduje nieco ine
  // alebo fModule na VirtualModule, potom nepotrebujem nikdy checkovat fMoudule


  if (ev != fEvent) Error("DecodeMHDR", "event mismatch %d != %d", ev, fEvent);
  if (gVME) {
    if (ga < gVME->Modules()->GetSize())
      fModule = gVME->GetModule(ga);
    else
      Error("DecodeMHDR", "slot (gate) %d out of bounds", ga);
  }
  if (fModule && (fModule->GetId() != id))
    Error("DecodeMHDR", "moduleID mismatch %d != %d", id, fModule->GetId());

  //  PrintWord(2);
  //  printf("MHDR ev: %d, id: %2d, ga: %2d\n", ev, id, ga);
}
//______________________________________________________________________________
void TRawData::DecodeMTRL()
{
  // 0x9 Module trailer
  //  Int_t wc  = fWord & 0xFFFF;       // (bits 0  - 15)
  Int_t er  = (fWord >> 16) & 0xF;  // (bits 16 - 19)
  //  Int_t crc = (fWord >> 20) & 0xFF; // (bits 20 - 27)

  if (er != 0xF) Error("DecodeMTRL", "module error 0x%X", er);

  //  PrintWord(2);
  //  printf("MTRL wc: %d, er: 0x%X, crc: 0x%X\n", wc, er, crc);

  if (!gVME) return;
  if (fModule) {
    //    fModule->MultiHitIdentify(fGemEvent);
    fModule->MultiHitClear();
  }
}
//______________________________________________________________________________
void TRawData::DecodeEHDR()
{
  // 0xA Event header
  fEvent = fWord & 0xFFFFF; // (bits 0 - 19)

  //  PrintWord(1);
  //  printf("EHDR ev: %d\n", fEvent);


  //  fGemEvent->Clear();

}
//______________________________________________________________________________
void TRawData::DecodeETRL()
{
  // 0xB Event trailer
  //  Int_t wc = fWord & 0xFFFFFF; // (bits 0 - 23)

  //  PrintWord(1);
  //  printf("ETRL wc: %d\n", wc);

  //  fGemEvent->SetEvent(fEvent);
  //  fTree->Fill();
}
//______________________________________________________________________________
void TRawData::DecodeSHDR()
{
  // 0xC Spill header
  Bool_t isEnd = (fWord >> 27) & 0x1; // (bit 27)
  if (!isEnd) fNSpills++;

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
void TRawData::DecodeStatus()
{
  // 0xE Status
  PrintWord(0);
  printf("Status\n");
}
//______________________________________________________________________________
void TRawData::DecodeReserved()
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
