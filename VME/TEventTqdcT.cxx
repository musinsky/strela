// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Mar 2015

#include "TEventTqdcT.h"
#include "THitTqdcT.h"
#include "TVME.h"
#include "TEventTdc.h"

ClassImp(TEventTqdcT)

//______________________________________________________________________________
TEventTqdcT::TEventTqdcT()
: TVirtualEvent(),
  fTrigTime(0)
{
  // Default constructor
  fHits = new TClonesArray(THitTqdcT::Class(), 400);

  if (!gVME) return;
  fIdxHitChanLast.Set(gVME->GetNChannelsTQDC());
  for (Int_t multi = 0; multi < kMaxMulti; multi++)
    fIdxHitChanMulti[multi].Set(fIdxHitChanLast.GetSize());
}
//______________________________________________________________________________
TEventTqdcT::~TEventTqdcT()
{
  //  Info("~TEventTqdcT", "Destructor");
}
//______________________________________________________________________________
void TEventTqdcT::Clear(Option_t *option)
{
  TVirtualEvent::Clear(option);
  fTrigTime = 0; // must be 0 (unless trigger)
}
//______________________________________________________________________________
void TEventTqdcT::Print(Option_t * /*option*/) const
{
  THitTqdcT *hit;
  Printf("next event");
  for (Int_t i = 0; i < fNHits; i++) {
    hit = GetHitTqdcT(i);
    Printf(" %2d) channel = %3d, T = %6d",
           i, hit->GetChannel(), hit->GetT());
  }
}
//______________________________________________________________________________
void TEventTqdcT::AddHitTqdcT(Int_t ch, Int_t t)
{
  // TDC data

  THitTqdcT *hit = (THitTqdcT *)fHits->ConstructedAt(fNHits++);
  hit->Set(ch, t);
}
//______________________________________________________________________________
Int_t TEventTqdcT::T(Int_t ch, Int_t multi)
{
  Int_t idx = GetIndexHit(ch, multi);
  if (idx == -1) return kNoneValue;

  Int_t t = GetHitTqdcT(idx)->GetT();
  if (TEventTdc::GetTrigChannel() < 0) return t;
  else return (t - fTrigTime + TEventTdc::GetTrigOffset());
}
