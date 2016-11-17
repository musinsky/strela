// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    17 Nov 2016

#include "THitTqdcT.h"

ClassImp(THitTqdcT)

//______________________________________________________________________________
void THitTqdcT::Set(Int_t ch, Int_t tld)
{
  fChannel = ch;
  fTime    = tld;
  fDelta   = 0;
}
//______________________________________________________________________________
void THitTqdcT::SetDelta(Int_t ttr)
{
  if (fTime > ttr) Warning("SetDelta", "TLD > TTR");
  else fDelta = ttr - fTime; // trailing - leading
}
