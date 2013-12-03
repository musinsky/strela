// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    04 Dec 2013

#include "TMultiHit.h"

ClassImp(TMHit)

//______________________________________________________________________________
TMHit::TMHit(Int_t time, Bool_t lead)
: fTime(time),
  fLead(lead)
{
  // Normal constructor
}
//______________________________________________________________________________
Int_t TMHit::Compare(const TObject *obj) const
{
  // without checking
  TMHit *hit = (TMHit *)obj;
  if (fTime < hit->GetTime()) return -1;
  else if (fTime > hit->GetTime()) return 1;
  else return 0;
}

//______________________________________________________________________________

ClassImp(TMultiHit)

//______________________________________________________________________________
TMultiHit::TMultiHit()
: fNHits(0),
  fHits(0)
{
  // Default constructor
  fHits = new TClonesArray("TMHit", 8*2);
}
//______________________________________________________________________________
TMultiHit::~TMultiHit()
{
  Info("~TMultiHit", "Destructor");
  if (fHits) fHits->Delete();
  delete fHits;
}
//______________________________________________________________________________
void TMultiHit::AddHit(Int_t time, Bool_t lead)
{
  TClonesArray &hits = *fHits;
  new(hits[fNHits++]) TMHit(time, lead);
}
//______________________________________________________________________________
void TMultiHit::Clear(Option_t *option)
{
  fNHits = 0;
  fHits->Clear(option);
}
//______________________________________________________________________________
void TMultiHit::Print(Option_t * /*option*/) const
{
  TMHit *hit;
  for (Int_t ih = 0; ih < GetNumOfHits(); ih++) {
    hit = GetHit(ih);
    Printf(" %2d) time = %6d, %s", ih, hit->GetTime(),
           hit->GetLead() ? "TLD" : "TTR");
  }
}
