// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    17 Apr 2014

#include "TTDCHit.h"

ClassImp(TTDCHit)

//______________________________________________________________________________
void TTDCHit::SetDelta(Int_t ttr)
{
  if (fTime > ttr) Warning("SetDelta", "TLD > TTR");
  else fDelta = ttr - fTime; // trailing - leading
}
