// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    28 Nov 2016

#include <TMath.h>

#include "TVME.h"
#include "TVirtualModule.h"
#include "THttpMonitor.h"

TVME *gVME = 0;
Bool_t TVME::fgReDecode = kFALSE;

ClassImp(TVME)

//______________________________________________________________________________
TVME::TVME()
: TNamed(),
  fModules(0),
  fNChannelsMSC(0),
  fNChannelsTQDC(0),
  fNChannels(0),
  fChannel(0),
  fIndexCha(0),
  fSortCha(0),
  fHttpMonitor(0)
{
  // Default constructor
}
//______________________________________________________________________________
TVME::TVME(const char *name, const char *title)
: TNamed(name, title),
  fModules(0),
  fNChannelsMSC(0),
  fNChannelsTQDC(0),
  fNChannels(0),
  fChannel(0),
  fIndexCha(0),
  fSortCha(0),
  fHttpMonitor(0)
{
  // Normal constructor
  if (gVME) {
    Error("TVME", "only one instance of TVME allowed");
    return;
  }

  gVME     = this;
  fModules = new TObjArray(20); // max slots (gates) in VME
}
//______________________________________________________________________________
TVME::~TVME()
{
  Info("~TVME", "Destructor");
  if (fModules) fModules->Delete();
  SafeDelete(fModules);
  DeleteChannels();
  if (gVME == this) gVME = 0;
}
//______________________________________________________________________________
void TVME::DeleteChannels()
{
  if (fChannel)  delete [] fChannel;
  if (fIndexCha) delete [] fIndexCha;
  if (fSortCha)  delete [] fSortCha;
}
//______________________________________________________________________________
void TVME::CountChannels()
{
  if (!fModules) return;

  TVirtualModule *module;
  fNChannelsMSC  = 0;
  fNChannelsTQDC = 0;
  fNChannels     = 0;

  for (Int_t im = 0; im < fModules->GetSize(); im++) {
    module = GetModule(im);
    if (!module) continue;
    if (module->IsMSC()) {
      module->SetFirstChannel(fNChannelsMSC);
      fNChannelsMSC += module->GetModuleNChannels();
    }
    else if (module->IsTQDC()) {
      module->SetFirstChannel(fNChannelsTQDC);
      fNChannelsTQDC += module->GetModuleNChannels();
    }
    else {
      module->SetFirstChannel(fNChannels);
      fNChannels += module->GetModuleNChannels();
    }
  }
}
//______________________________________________________________________________
void TVME::ReDecodeChannels()
{
  if (!fModules) return;

  CountChannels();
  if (fNChannels == 0) return; // only other than TDC channels

  // from 2013_12 is no longer needed, but leaving feedback over fgReDecode
  //
  // for 2007_03, 2008_06, 2009_12, 2011_03 see in pomme.cxx
  Int_t moduleShift = 100 - 512, moduleDelta = 512, idMulti = 32;

  DeleteChannels();
  fChannel  = new Int_t[fNChannels];
  fIndexCha = new Int_t[fNChannels];
  fSortCha  = new Int_t[fNChannels];

  TVirtualModule *module;
  Int_t first, tdcId, tdcCh, inadc, nadc;
  for (Int_t im = 0; im < fModules->GetSize(); im++) {
    module = GetModule(im);
    if (!module) continue;
    moduleShift += moduleDelta;
    first = module->GetFirstChannel();
    for (Int_t ich = 0; ich < module->GetModuleNChannels(); ich++) {
      inadc = first + ich;
      module->GetChannelIdCh(ich, tdcId, tdcCh);
      nadc = moduleShift + (tdcCh + tdcId*idMulti);
      if (fgReDecode) fChannel[inadc] = nadc;  // old
      else            fChannel[inadc] = inadc; // new
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
//______________________________________________________________________________
void TVME::RegisterHttpMonitor(THttpMonitor *hm)
{
  if (!hm) return;
  if (fHttpMonitor) {
    Error("RegisterHttpMonitor", "only one instance of THttpMonitor allowed");
  }
  fHttpMonitor = hm;
}
//______________________________________________________________________________
void TVME::SwitchResetCycle()
{
  if (fHttpMonitor) fHttpMonitor->SwitchResetCycle();
}
//______________________________________________________________________________
void TVME::ResetMonitors(const char *arg1)
{
  if (fHttpMonitor) fHttpMonitor->ResetMonitors(arg1);
}
