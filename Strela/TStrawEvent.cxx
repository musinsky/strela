// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    24 Oct 2007

#include "TStrawEvent.h"

ClassImp(TStrawHit)

//______________________________________________________________________________

ClassImp(TStrawEvent)

//______________________________________________________________________________
TStrawEvent::TStrawEvent()
{
  //  Info("TStrawEvent", "Default constructor");
  fNevent  = 0;
  fNstraws = 0;
  fStraws  = new TClonesArray("TStrawHit", 50);
}
//______________________________________________________________________________
TStrawEvent::~TStrawEvent()
{
  Info("~TStrawEvent", "Destructor");
  if (fStraws) fStraws->Delete();
  delete fStraws;
}
