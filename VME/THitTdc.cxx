// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Mar 2015

#include "THitTdc.h"

ClassImp(THitTdc)

//______________________________________________________________________________
void THitTdc::Set(Int_t ch, Int_t tld)
{
  fChannel = ch;
  fTime    = tld;
  fDelta   = 0;
}
//______________________________________________________________________________
void THitTdc::SetDelta(Int_t ttr)
{
  if (fTime > ttr) Warning("SetDelta", "TLD > TTR");
  else fDelta = ttr - fTime; // trailing - leading
}
