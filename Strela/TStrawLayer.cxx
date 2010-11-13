// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 26 May 2008

#include "TStrawLayer.h"
#include "TStrawTube.h"

ClassImp(TStrawLayer)

//______________________________________________________________________________
TStrawLayer::TStrawLayer()
{
  //  Info("TStrawLayer", "Default constructor");
  fTubes   = 0;
  fZ       = 0;
  fRange   = 0;
  fNumb    = -1;
  fTracker = 0;
}
//______________________________________________________________________________
TStrawLayer::TStrawLayer(Int_t numb, const char *name, const char *title)
  : TNamed(name, title)
{
  //  Info("TStrawLayer", "Normal Constructor");
  fTubes   = 0;
  fZ       = 0;
  fRange   = 0;
  fNumb    = numb;
  fTracker = 0;
}
//______________________________________________________________________________
TStrawLayer::~TStrawLayer()
{
  //  Info("~TStrawLayer", "Destructor");
  if (fTubes) fTubes->Delete();
  delete fTubes;
  fTubes = 0;
}
//______________________________________________________________________________
Int_t TStrawLayer::Compare(const TObject *obj) const
{
  if (fZ < ((TStrawLayer *)obj)->GetZ()) return -1;
  else if (fZ > ((TStrawLayer *)obj)->GetZ()) return 1;
  else return 0;
}
//______________________________________________________________________________
void TStrawLayer::MakeTubes(Int_t n, Double_t del, Int_t dir)
{
  if (fTubes) {
    Error("MakeTubes", "tubes in layer %s is an already existing", GetName());
    return;
  }

  // origin is center of layer
  // X increase from right to left and Y increase from down to up
  Double_t first = 0.0 - ((n - 1)*del)/2.0;
  fTubes = new TList();
  for (Int_t i = 0; i < n; i++) {
    TStrawTube *tube = new TStrawTube((first + i*del)*dir);
    fTubes->AddAt(tube, i); // do not sort this list, must stay this order
    tube->SetLayer(this);
  }
}
//______________________________________________________________________________
void TStrawLayer::Shift(Double_t shift) const
{
  if (!fTubes) {
    Error("Shift", "tubes in layer %s does not exist", GetName());
    return;
  }

  for (Int_t i = 0; i < fTubes->GetSize(); i++) {
    TStrawTube *tube = (TStrawTube *)fTubes->At(i);
    tube->SetCenter(tube->GetCenter() + shift);
  }
}
