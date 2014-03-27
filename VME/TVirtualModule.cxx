// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    06 Dec 2013

#include "TVirtualModule.h"
#include "TVME.h"
#include "TMultiHit.h"
#include "TGemEvent.h"

ClassImp(TVirtualModule)

//______________________________________________________________________________
TVirtualModule::TVirtualModule()
: TObject(),
  fId(-1),
  fSlot(-1),
  fNChips(0),
  fChipNChannels(0),
  fFirstChannel(-1),
  fMultiHits(0)
{
  // Default constructor
}
//______________________________________________________________________________
TVirtualModule::~TVirtualModule()
{
  Info("~TVirtualModule", "Destructor");
  if (gVME) gVME->Modules()->Remove(this);
  if (fMultiHits) fMultiHits->Delete();
  delete fMultiHits;
}
//______________________________________________________________________________
const char *TVirtualModule::GetTitle() const
{
  return Form("%s (Id: %02d) in slot: %02d", GetName(), fId, fSlot);
}
//______________________________________________________________________________
void TVirtualModule::VMEModule(Int_t slot)
{
  if (!gVME) {
    Error("VMEModule", "gVME not initialized");
    return;
  }
  if (gVME->Modules()->At(slot)) {
    Error("VMEModule", "slot %d is already occupied", slot);
    return;
  }

  fSlot = slot;
  gVME->Modules()->AddAt(this, fSlot);
  gVME->ReDecodeChannels(); // and SetFirstChannel for ! each ! modules

  // temporary, !!! memory leak !!!
  fMultiHits = new TObjArray(GetModuleNChannels());
  for (Int_t i = 0; i < fMultiHits->GetSize(); i++)
    fMultiHits->AddAt(new TMultiHit(), i);
}
//______________________________________________________________________________
Int_t TVirtualModule::MapChannel(Int_t /*tdcid*/, Int_t /*tdcch*/) const
{
  // override this function
  return -1;
}
//______________________________________________________________________________
Bool_t TVirtualModule::GetChannelIdCh(Int_t /*ch*/, Int_t & /*tdcid*/, Int_t & /*tdcch*/) const
{
  // override this function
  return kFALSE;
}
//______________________________________________________________________________
void TVirtualModule::ConnectorChannels(Int_t con, Int_t * /*pins*/, Option_t * /*option*/) const
{
  if ((fFirstChannel < 0) || (fSlot < 0))
    Error("ConnectorChannels", "module %s is not in list of modules", GetTitle());
  if (con >= fNChips)
    Error("ConnectorChannels", "module %s has only %d chips", GetTitle(), fNChips);
}
//______________________________________________________________________________
Int_t TVirtualModule::DecodeChannel(UInt_t word) const
{
  UInt_t type = word >> 28;
  if ((type != 0x4) && (type != 0x5)) {
    Warning("DecodeChannel", "decode only DATA (0x4 or 0x5), not: 0x%X", type);
    return -1;
  }
  // tdc_id = bits 24 - 27, tdc_ch = bits 19 - 23
  return MapChannel((word >> 24) & 0xF, (word >> 19) & 0x1F);
}
//______________________________________________________________________________
void TVirtualModule::MultiHitAdd(Int_t nadc, Int_t time, Bool_t lead)
{
  if (!fMultiHits) return;

  if (nadc != -1) GetMultiHit(nadc)->AddHit(time, lead);
}
//______________________________________________________________________________
void TVirtualModule::MultiHitClear() const
{
  if (!fMultiHits) return;

  TMultiHit *mh;
  for (Int_t i = 0; i < fMultiHits->GetSize(); i++) {
    mh = GetMultiHit(i);
    if (mh->GetNHits()) mh->Clear();
  }
}
//______________________________________________________________________________
void TVirtualModule::MultiHitIdentify(TGemEvent *event) const
{
  if (!event) return;
  if (!fMultiHits) return;

  TMultiHit *mh;
  TMHit *hit1, *hit2;
  Int_t nhits;

  // check on first !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  for (Int_t i = 0; i < fMultiHits->GetSize(); i++) {
    mh = GetMultiHit(i);
    nhits = mh->GetNHits();
    if (nhits == 0) continue;

    if (nhits == 1) {
      hit1 = mh->GetHit(0);
      if (hit1->GetLead()) {
        // only one leading hit
        // time = hit1->GetTime();
        // delta = 0;
        // Printf("type A: %6d", hit1->GetTime());
        event->AddHit1(fFirstChannel+i, hit1->GetTime(), 0);
      }
      continue;
    }

    mh->Hits()->Sort(); // TODO: is really necessary sorting by time
    for (Int_t j = 1; j < nhits; j++) {
      hit1 = mh->GetHit(j-1);
      hit2 = mh->GetHit(j);
      if (hit1->GetLead() && !hit2->GetLead()) {
        // leading before trailing
        // time = hit1->GetTime();
        // delta = hit2->GetTime() - hit1->GetTime();
        // Printf("type B: %6d, %6d", hit1->GetTime(), hit2->GetTime());
        event->AddHit1(fFirstChannel+i, hit1->GetTime(), hit2->GetTime()-hit1->GetTime()); //!!!!!!!!!! check on minus, sort
      }
      else if (hit1->GetLead()) {
        // leading before leading
        // time = hit1->GetTime();
        // delta = 0;
        // Printf("type C: %6d", hit1->GetTime());
        event->AddHit1(fFirstChannel+i, hit1->GetTime(), 0);
      }
      if ((j == (nhits-1)) && hit2->GetLead()) { // dont use else if
        // last leading hit
        // time = hit2->GetTime();
        // delta = 0;
        // Printf("type D: %6d", hit2->GetTime());
        event->AddHit1(fFirstChannel+i, hit2->GetTime(), 0);
      }
    }
  }
}
