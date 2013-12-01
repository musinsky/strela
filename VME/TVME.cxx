// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    01 Dec 2013

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <TMath.h>

#include "TVME.h"
#include "TVirtualModule.h"
#include "TModulePhTDC.h"
#include "TModuleTDC96.h"
#include "TModuleTDC64V.h"

TVME *gVME = 0;

ClassImp(TVME)

//______________________________________________________________________________
TVME::TVME()
{
  //  Info("TVME", "Default constructor");
  fModules   = 0;
  fNChannels = 0;
  fChannel   = 0;
  fIndexCha  = 0;
  fSortCha   = 0;
}
//______________________________________________________________________________
TVME::TVME(const char *name, const char *title) : TNamed(name, title)
{
  //  Info("TVME", "Normal constructor");
  gVME       = this;
  fModules   = new TObjArray(16); // max slots (gates) in VME
  fNChannels = 0;
  fChannel   = 0;
  fIndexCha  = 0;
  fSortCha   = 0;
}
//______________________________________________________________________________
TVME::~TVME()
{
  Info("~TVME", "Destructor");
  if (fModules) fModules->Delete();
  delete fModules;
  fModules = 0;
  DeleteChannels();
  gVME = 0;
}
//______________________________________________________________________________
void TVME::DeleteChannels()
{
  fNChannels = 0;
  delete [] fChannel;
  delete [] fIndexCha;
  delete [] fSortCha;
}
//______________________________________________________________________________
Int_t TVME::GetNChannels() const
{
  TVirtualModule *module;
  Int_t sum = 0;
  for (Int_t im = 0; im < fModules->GetSize(); im++) {
    module = GetModule(im);
    if (!module) continue;
    sum += module->GetModuleNChannels();
  }
  return sum;
}
//______________________________________________________________________________
Int_t TVME::FirstChannelOfModule(const TVirtualModule *mod) const
{
  TVirtualModule *module;
  Int_t first = 0;
  for (Int_t im = 0; im < fModules->GetSize(); im++) {
    module = GetModule(im);
    if (!module) continue;
    if (mod == module) return first;
    first += module->GetModuleNChannels();
  }
  return -1;
}
//______________________________________________________________________________
void TVME::ReDecodeChannels()
{
  // see in pomme.cxx
  // 2007_03, 2008_06
  Int_t moduleShift = 100 - 512, moduleDelta = 512, idMulti = 32;

  DeleteChannels();
  fNChannels = GetNChannels();
  fChannel  = new Int_t[fNChannels];
  fIndexCha = new Int_t[fNChannels];
  fSortCha  = new Int_t[fNChannels];

  TVirtualModule *module;
  Int_t first, tdcId, tdcCh, inadc, nadc;
  for (Int_t im = 0; im < fModules->GetSize(); im++) {
    module = GetModule(im);
    if (!module) continue;
    moduleShift += moduleDelta;
    first = FirstChannelOfModule(module);
    for (Int_t ich = 0; ich < module->GetModuleNChannels(); ich++) {
      inadc = first + ich;
      module->GetChannelIdCh(ich, tdcId, tdcCh);
      nadc = moduleShift + (tdcCh + tdcId*idMulti);
      fChannel[inadc] = nadc;
      if (gDebug > 0) Printf("[%3d] %4d", inadc, nadc);
    }
  }

  // sorting by increasing nadc
  TMath::Sort(fNChannels, fChannel, fIndexCha, kFALSE);
  for (Int_t i = 0; i < fNChannels; i++) fSortCha[i] = fChannel[fIndexCha[i]];
}
//______________________________________________________________________________
Int_t TVME::SearchChannel(Int_t nadc) const
{
  Int_t pos = TMath::BinarySearch(fNChannels, fSortCha, nadc);
  if ((pos >= 0) && (fSortCha[pos] == nadc)) return fIndexCha[pos];
  return -1;
}
//______________________________________________________________________________
const char *TVME::GetChannelInfo(Int_t nadc) const
{
  return Form("[%3d][%4d]", SearchChannel(nadc), nadc);
}
//
//______________________________________________________________________________
//
void TVME::DecodeFile(const char *fname, Int_t ne, Int_t imod) const
{
  Int_t fd, nspills = 0, nevents = 0, nmodules = 0, nhits = 0;
  UInt_t word;

  fd = open(fname, O_RDONLY);
  if (fd == -1) {
    Error("DecodeFile", "file %s can not be opened", fname);
    return;
  }

  while (read(fd, &word, sizeof(word)) == sizeof(word)) {

    // 0xC Spill header
    if ((word >> 28) == 0xC) {
      printf("SHDR #%d\n", nspills);
      continue;
    }

    // 0xA Event header
    if ((word >> 28) == 0xA) {
      if (ne && nevents >= ne) break; // only first ne events
      printf("\tEHDR #%d", nevents);
      printf(", event number: %d\n", word & 0xFFFFF); // (bits 0 - 19)
      nmodules = 0;
      nhits    = 0;
      continue;
    }

    // 0x8 Module header
    if ((word >> 28) == 0x8) {
      printf("\t\tMHDR #%d", nmodules);
      // Custom VME Module Specifications (not default)
      printf(", event count: %d", word & 0xFFFF); // (bits 0  - 15)
      printf(", id: %d", (word >> 16) & 0x7F);    // (bits 16 - 22)
      printf(", ga: %d\n", (word >> 23) & 0x1F);  // (bits 23 - 27)
      continue;
    }

    // 0x4 TDC leading edge measurement
    if ((word >> 28) == 0x4) {
      if ((imod != -1) && (imod != nmodules)) continue; // only one imod module
      printf("\t\t\tDATA #%d\n", nhits);
      printf("\t\t\ttdc_id:%2d", (word >> 24) & 0xF); // (bits 24 - 27)
      printf(", tdc_ch:%3d", (word >> 19) & 0x1F);    // (bits 19 - 23)
      printf(", time: %d\n", word & 0x7FFFF);         // (bits 0  - 18)
      nhits++;
      continue;
    }

    // 0x9 Module trailer
    if ((word >> 28) == 0x9) {
      printf("\t\tMTRL #%d", nmodules);
      // Custom VME Module Specifications (not default)
      printf(", word count: %d", word & 0xFFFF); // (bits 0  - 15)
      if (((word >> 16) & 0xF) != 0xF)           // (bits 16 - 19)
        printf("\n\033[31m MTRL bits (16 - 19) are not 1111\033[m\n");
      printf(", flag: %d", (word >> 20) & 0x7);  // (bits 20 - 22)
      printf(", ga: %d\n", (word >> 23) & 0x1F); // (bits 23 - 27)
      nmodules++;
      continue;
    }

    // 0xB Event trailer
    if ((word >> 28) == 0xB) {
      printf("\tETRL #%d", nevents);
      printf(", event wordcount: %d", word & 0xFFFF);    // (bits 0  - 15)
      printf(", xoff count: %d\n", (word >> 16) & 0xFF); // (bits 16 - 23)
      nevents++;
      continue;
    }

    // 0xD Spill trailer
    if ((word >> 28) == 0xD) {
      printf("STRL #%d", nspills);
      printf(", spill wordcount: %d\n", word & 0xFFFFFFF); // (bits 0 - 27)
      nspills++;
      continue;
    }
  }
  close(fd);
}
//______________________________________________________________________________
void TVME::RawParser(const char *fname) const
{
  TVirtualModule *module;
}
