// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    12 Sep 2012

#include <TCanvas.h>
#include <TButton.h>
#include <TFile.h>
#include <TLine.h>

#include "TStrelaDisplay.h"
#include "TStrawTracker.h"
#include "TStrawTrack.h"

const UInt_t kMinID = 1000;

ClassImp(TStrelaDisplay)

//______________________________________________________________________________
TStrelaDisplay::TStrelaDisplay(Int_t xsize)
{
  //  Info("TStrelaDisplay", "Default constructor");
  if (!gStrela) return;

  fCanvas = new TCanvas("canvas", "STRELA Display", 0, 0, xsize, Int_t(xsize/1.5));
  fPad = new TPad("pad", "Detectors display", 0.00, 0.07, 1.00, 1.00);
  fPad->Connect("Closed()", "TStrelaDisplay", this, "~TStrelaDisplay()");
  fPad->Draw();
  DisplayButtons();
  AppendPad();
  fPad->cd();
  fCanvas->Update();
  gStrela->SetDisplay(this); // must be after fPad->Connect
}
//______________________________________________________________________________
TStrelaDisplay::~TStrelaDisplay()
{
  //  Info("~TStrelaDisplay", "Destructor");
  fCanvas = 0;
  fPad    = 0;
  if (gStrela) gStrela->SetDisplay(0);
}
//______________________________________________________________________________
void TStrelaDisplay::DisplayButtons() const
{
  fCanvas->cd();
  TPad *buttPad = new TPad("buttons", "Buttons", 0.00, 0.00, 1.00, 0.07);
  buttPad->SetFillColor(33);
  buttPad->Draw();
  buttPad->cd();

  TButton *button;
  Double_t y1 = 0.10, y2 = 0.90;

  const char *but1 = "gStrela->GetDisplay()->DisplayNextEvent(1)";
  button = new TButton("Next", but1, 0.01, y1, 0.11, y2);
  button->SetFillColor(kGreen - 5);
  button->Draw();

  const char *but2 = "gStrela->GetDisplay()->DisplayNextEvent(-1)";
  button = new TButton("Prev", but2, 0.13, y1, 0.23, y2);
  button->SetFillColor(kGreen - 5);
  button->Draw();

  const char *but3 = "gStrela->GetDisplay()->ReDraw()";
  button = new TButton("ReDraw", but3, 1.00 - 0.01, y1, 1.00 - 0.11, y2);
  button->SetFillColor(kGreen - 8);
  button->Draw();

  buttPad->SetEditable(kFALSE);
}
//______________________________________________________________________________
void TStrelaDisplay::DrawTracker(const char *name, Option_t *option) const
{
  TStrawTracker *tracker = (TStrawTracker *)gStrela->StrawCham()->Trackers()->FindObject(name);
  if (!tracker) {
    Error("DrawTracker", "tracker %s does not exist", name);
    return;
  }
  DrawTracker(tracker, option);
}
//______________________________________________________________________________
void TStrelaDisplay::DrawTracker(TStrawTracker *tracker, Option_t *option) const
{
  TString opt = option;
  if (gPad && !opt.Contains("same")) gPad->Clear();
  tracker->AppendPad(option); // add to gPad ListOfPrimitives

  TStrawCham *scham = gStrela->StrawCham();
  TStrawTube *tube;
  for (Int_t i = 0; i < scham->Tubes()->GetEntriesFast(); i++) {
    tube = scham->GetTube(i);
    if (tube->GetTracker() != tracker) continue;
    if (tube->GetNadc() < 0) tube->SetLineStyle(10); // wrong nadc
    else {
      if (strstr(tube->GetLayer()->GetName(), "strah_"))
        tube->SetLineStyle(1);
      else if (strstr(tube->GetLayer()->GetName(), "dch_"))
        tube->SetLineStyle(2);
      else tube->SetLineStyle(10); // unknown type
    }
    tube->SetFillStyle(3003);
    tube->Draw(option);
  }

  FindRange();
}
//______________________________________________________________________________
void TStrelaDisplay::FindRange(Double_t pm) const
{
  if (!gPad) return;

  Double_t x1 = 9999, y1 = x1;
  Double_t x2 =  -x1, y2 = x2;
  TObject *obj;
  TStrawTube *tube;
  TIter next(gPad->GetListOfPrimitives());

  while ((obj = next())) {
    if (!obj->InheritsFrom(TStrawTube::Class())) continue;
    tube = (TStrawTube *)obj;
    x1 = TMath::Min(tube->GetX1() - tube->GetR1(), x1);
    y1 = TMath::Min(tube->GetY1() - tube->GetR1(), y1);
    x2 = TMath::Max(tube->GetX1() + tube->GetR1(), x2);
    y2 = TMath::Max(tube->GetY1() + tube->GetR1(), y2);
  }
  if (x1 > x2) {
    Info("FindRange", "cannot find any tubes");
    return;
  }

  Double_t dx = TMath::Abs(x1 - x2)*pm;
  Double_t dy = TMath::Abs(y1 - y2)*pm;
  gPad->Range(x1 - dx, y1 - dy, x2 + dx, y2 + dy);
  gPad->Modified();
}
//______________________________________________________________________________
void TStrelaDisplay::DisplayEvent(Option_t *option) const
{
  // show event with hits and tracks on all displayed tracker(s)
  if (!gPad) return;

  Bool_t clearTracker = kTRUE;
  TObject *obj;
  TStrawTracker *tracker;
  TIter next(gPad->GetListOfPrimitives());

  // 1) checking all tracker(s) on pad(s)
  while ((obj = next())) {
    if (!obj->InheritsFrom(TStrawTracker::Class())) continue;
    tracker = (TStrawTracker *)obj;
    if (clearTracker) DeletePrimitives();
    clearTracker = kFALSE;
    DisplayEvent(tracker, option);
  }

  // 2) checking all subpad(s)
  next.Reset();
  while ((obj = next())) {
    if (obj->IsA() != TPad::Class()) continue; // only TPad
    ((TPad *)obj)->cd();
    DisplayEvent(option); // recursive calls
  }
}
//______________________________________________________________________________
void TStrelaDisplay::DisplayEvent(const TStrawTracker *tracker, Option_t * /*option*/) const
{
  if (!gPad) return;

  Int_t indexMark[tracker->GetNHits()];
  TEllipse eh;
  eh.SetFillStyle(0);
  eh.SetLineColor(kRed);
  for (Int_t i = 0; i < tracker->GetNHits(); i++) {
    // eh.SetBit(kCanDelete);
    eh.DrawEllipse(tracker->Z(i), tracker->X(i), tracker->R(i), tracker->R(i), 0, 360, 0);
    gPad->GetListOfPrimitives()->Last()->SetUniqueID(kMinID + 1);
    indexMark[i] = gPad->GetListOfPrimitives()->LastIndex();
  }

  TLine line;
  Double_t x1 = gPad->GetX1();
  Double_t x2 = gPad->GetX2();
  Double_t xd = TMath::Abs(x2 - x1);
  x1 += 0.05*xd;
  x2 -= 0.05*xd;
  for (Int_t i = 0; i < gStrela->StrawCham()->GetNumOfTracks(); i++) {
    TStrawTrack *track = gStrela->StrawCham()->GetStrawTrack(i);
    if (track->GetId() != tracker->GetId()) continue;

    if (track->GetNHits() == 0) { // tangent (debug mode)
      line.SetLineColor(kRed);
      line.SetLineStyle(3);
    }
    else {                      // real track
      line.SetLineColor(kBlue);
      line.SetLineStyle(1);
    }
    // line.SetBit(kCanDelete);
    line.DrawLine(x1, x1*track->GetAz() + track->GetBz(), x2, x2*track->GetAz() + track->GetBz());
    gPad->GetListOfPrimitives()->Last()->SetUniqueID(kMinID + 2);

    // mark track hits
    TObject *obj;
    for (Int_t ih = 0; ih < track->GetNHits(); ih++) {
      obj = gPad->GetListOfPrimitives()->At(indexMark[track->GetHit(ih)]);
      if (!obj || (obj->IsA() != TEllipse::Class())) {
        Error("DisplayEvent", "hit %d not found", track->GetHit(ih));
        continue;
      }
      ((TEllipse *)obj)->SetLineColor(line.GetLineColor());
    }
  }
}
//______________________________________________________________________________
void TStrelaDisplay::DeletePrimitives() const
{
  TObject *obj;
  TIter next(gPad->GetListOfPrimitives());

  while ((obj = next()))
    if (obj->GetUniqueID() > kMinID) delete obj;
}
//______________________________________________________________________________
void TStrelaDisplay::DisplayNextEvent(Int_t delta) const
{
  if (!gPad) return;
  if (!gStrela->GetChain()) return;

  fPad->cd();
  Int_t readEntry = gStrela->GetChain()->GetReadEntry();
  readEntry += delta;
  gStrela->AnalyzeEntry(readEntry);
  fCanvas->SetTitle(gStrela->GetEventInfo());
  DisplayEvent();
}
//______________________________________________________________________________
void TStrelaDisplay::ReDraw() const
{
  if (!gPad) return;

  TList trackers;
  TObject *obj;

  TIter nextp(gPad->GetListOfPrimitives());
  while ((obj = nextp()))
    if (obj->InheritsFrom(TStrawTracker::Class())) trackers.Add(obj);

  if (trackers.IsEmpty()) return;
  gPad->Clear();
  TIter nextt(&trackers);
  while ((obj = nextt()))
    DrawTracker((TStrawTracker *)obj, "same");

  DisplayNextEvent(0);
}
//______________________________________________________________________________
TVirtualPad *TStrelaDisplay::FindPad(const TObject *obj, Bool_t update, TVirtualPad *pad) const
{
  if (!pad) pad = fPad;
  TList *primitives = pad->GetListOfPrimitives();
  if (primitives->FindObject(obj)) {
    if (update) {
      pad->Modified();
      pad->Update();
    }
    return pad;
  }

  TObject *cur;
  TVirtualPad *found;
  TIter next(primitives);
  while ((cur = next())) {
    if (cur->InheritsFrom(TVirtualPad::Class())) {
      found = FindPad(obj, update, (TVirtualPad *)cur); // recursive calls
      if (found) return found;
    }
  }

  return 0;
}
