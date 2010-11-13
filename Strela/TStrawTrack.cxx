// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 16 may 2008

#include <string.h>

#include "TStrawTrack.h"

ClassImp(TStrawTrack)

//______________________________________________________________________________
TStrawTrack::TStrawTrack(Int_t id, Double32_t a, Double32_t b, Double32_t chi2)
{
  fId   = id;
  fAz   = a;
  fBz   = b;
  fChi2 = chi2;

  fNHits = 0;
}
//______________________________________________________________________________
void TStrawTrack::SetHits(Int_t nhits, const Int_t *hit)
{
  // without streaming hit information, need only for displaying
  // otherwise fHit must be declare as pointer and use new/delete operations

  if (nhits >= kMAXHIT) {
    Error("SetHits", "to many hits, limit is %d", kMAXHIT);
    fNHits = 0;
    return;
  }

  fNHits = nhits;
  memcpy(fHit, hit, fNHits*sizeof(Int_t));
}
