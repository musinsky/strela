// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    27 Feb 2015

#include "TTDCHit.h"

ClassImp(TTDCHit)

//______________________________________________________________________________
void TTDCHit::Set(Int_t ch, Int_t tld)
{
  fChannel = ch;
  fTime    = tld;
  fDelta   = 0;
}
//______________________________________________________________________________
void TTDCHit::SetDelta(Int_t ttr)
{
  if (fTime > ttr) Warning("SetDelta", "TLD > TTR");
  else fDelta = ttr - fTime; // trailing - leading
}
