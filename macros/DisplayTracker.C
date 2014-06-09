// Author: Jan Musinsky
// 09/06/2014

#include <TPad.h>

#include "TStrela.h"
#include "TStrelaDisplay.h"

void DisplayTracker(TStrawTracker *trX_1, TStrawTracker *trX_2, TStrawTracker *trX_3,
                    TStrawTracker *trY_1, TStrawTracker *trY_2, TStrawTracker *trY_3)
{
  // all trackers
  if (gStrela->GetDisplay()) return;
  TStrelaDisplay *display = new TStrelaDisplay();

  Double_t xl[3] = {0.00, 0.25, 0.75};
  Double_t xu[3] = {0.25, 0.75, 1.00};
  Double_t yl[3] = {0.1072, 0.192, 0.000};
  Double_t yu[3] = {0.3929, 0.308, 0.500};
  TPad *pad;
  Int_t npad = 1; // must start from 1
  TPad *pad0 = display->GetPad();

  for (Int_t iy = 0; iy < 2; iy++) {
    for (Int_t ix = 0; ix < 3; ix++) {
      yl[ix] = iy*0.5 + yl[ix];
      yu[ix] = iy*0.5 + yu[ix];
      pad = new TPad(Form("%s_%d", pad0->GetName(), npad), "",
                     xl[ix], yl[ix], xu[ix], yu[ix]);
      pad->SetNumber(npad++);
      pad->Draw();
    }
  }
  // special pad_7
  pad = new TPad(Form("%s_%d", pad0->GetName(), npad), "",
                 0.30, yl[0], 0.50, yu[0]);
  pad->SetNumber(npad++);
  pad->Draw();

  if (trY_1) {
    pad0->cd(1);
    display->DrawTracker(trY_1);
  }
  if (trY_2) {
    pad0->cd(2);
    display->DrawTracker(trY_2);
  }
  if (trY_3) {
    pad0->cd(3);
    display->DrawTracker(trY_3);
  }
  if (trX_1) {
    pad0->cd(4);
    display->DrawTracker(trX_1);
  }
  if (trX_2) {
    pad0->cd(7);
    display->DrawTracker(trX_2);
    //    display->FindRange(0.006);
  }
  if (trX_3) {
    pad0->cd(6);
    display->DrawTracker(trX_3);
  }

  pad0->cd(0);
}

void DisplayTracker(Int_t nx, Int_t ny, TStrawTracker *tr1, TStrawTracker *tr2,
                    TStrawTracker *tr3 = 0, TStrawTracker *tr4 = 0)
{
  // (double)pair of trackers
  if (gStrela->GetDisplay()) return;
  TStrelaDisplay *display = new TStrelaDisplay();

  TPad *pad0 = display->GetPad();
  pad0->Divide(nx, ny, 0.0, 0.0);
  pad0->cd(1);
  display->DrawTracker(tr1);
  pad0->cd(2);
  display->DrawTracker(tr2);
  if (tr3) {
    pad0->cd(3);
    display->DrawTracker(tr3);
  }
  if (tr4) {
    pad0->cd(4);
    display->DrawTracker(tr4);
  }
  pad0->cd(0);
}

void DisplayTracker(TStrawTracker *tr)
{
  // one tracker
  if (gStrela->GetDisplay()) return;
  TStrelaDisplay *display = new TStrelaDisplay();

  display->DrawTracker(tr);
  display->GetPad()->cd(0);
}
