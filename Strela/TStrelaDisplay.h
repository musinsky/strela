// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 24 Oct 2007

#ifndef STRELA_TStrelaDisplay
#define STRELA_TStrelaDisplay

#ifndef ROOT_TObject
#include <TObject.h>
#endif

class TCanvas;
class TPad;

class TStrawTracker;

class TStrelaDisplay : public TObject {

private:
  TCanvas      *fCanvas;    //  pointer to display canvas
  TPad         *fPad;       //  pointer to detectors pad

public:
  TStrelaDisplay(Int_t xsize = 650);
  virtual      ~TStrelaDisplay();

  TCanvas      *GetCanvas() const { return fCanvas; }
  TPad         *GetPad() const { return fPad; }

  void          DisplayButtons() const;
  void          DrawTracker(const char *name, Option_t *option = "") const;
  void          DrawTracker(TStrawTracker *tracker, Option_t *option = "")const;
  void          FindRange(Double_t pm = 0.10) const;
  void          DisplayEvent(Option_t *option = "") const;
  void          DisplayEvent(const TStrawTracker *tracker,
                             Option_t *option = "") const;
  void          DeletePrimitives() const;
  void          DisplayNextEvent(Int_t delta) const;
  void          ReDraw() const;

  ClassDef(TStrelaDisplay, 0) // StrelaDisplay
};

#endif
