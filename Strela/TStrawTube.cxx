// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 17 Jun 2008

#include <TH2.h>
#include <TSpline.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TGClient.h>

#include "TStrawTube.h"
#include "TStrawTracker.h"
#include "TStrawMulti.h"
#include "TVME.h"

Double_t TStrawTube::fgWireRadius     = 0.0025/2;
Int_t    TStrawTube::fgShowHistograms = 0;

ClassImp(TStrawTube)

//______________________________________________________________________________
TStrawTube::TStrawTube()
{
  //  Info("TStrawTube", "Default constructor");
  fCenter   = 0;
  fT0       = 0;
  fTMin     = 0;
  fTMax     = 0;
  fDriftVel = 0;
  fNadc     = -1;
  fLayer    = 0;
  fMulti    = 0;
  fCutT1    = 0;
  fCutT2    = 0;
  fhTime    = 0;
  fhCutTime = 0;
  fhRadius  = 0;
  fhDis1    = 0;
  fhDis2    = 0;
  fhEffi    = 0;
  fhTimeRes = 0;
  fhDisTime = 0;
}
//______________________________________________________________________________
TStrawTube::TStrawTube(Double_t center) : TEllipse()
{
  //  Info("TStrawTube", "Normal constructor");
  fCenter  = center;
  fT0       = 0;
  fTMin     = 0;
  fTMax     = 0;
  fDriftVel = 0;
  fNadc     = -1;
  fLayer    = 0;
  fMulti    = 0;
  fCutT1    = 0;
  fCutT2    = 0;
  fhTime    = 0;
  fhCutTime = 0;
  fhRadius  = 0;
  fhDis1    = 0;
  fhDis2    = 0;
  fhEffi    = 0;
  fhTimeRes = 0;
  fhDisTime = 0;
}
//______________________________________________________________________________
TStrawTube::~TStrawTube()
{
  //  Info("~TStrawTube", "Destructor");
  delete fhTime;    fhTime = 0;
  delete fhCutTime; fhCutTime = 0;
  delete fhRadius;  fhRadius = 0;
  delete fhDis1;    fhDis1 = 0;
  delete fhDis2;    fhDis2 = 0;
  delete fhEffi;    fhEffi = 0;
  delete fhTimeRes; fhTimeRes = 0;
  delete fhDisTime; fhDisTime = 0;
}
//______________________________________________________________________________
Int_t TStrawTube::Compare(const TObject *obj) const
{
  if (fNadc < ((TStrawTube *)obj)->GetNadc()) return -1;
  else if (fNadc > ((TStrawTube *)obj)->GetNadc()) return 1;
  else return 0;
}
//______________________________________________________________________________
void TStrawTube::Draw(Option_t *option)
{
  if (!fLayer) return;

  fX1 = GetZ();
  fY1 = fCenter;
  fR1 = fR2 = GetRange();
  AppendPad(option);
}
//______________________________________________________________________________
void TStrawTube::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
  if (!fgShowHistograms) return;

  switch (event) {

  case kMouseEnter:
    ShowHistograms();
    if (!fgShowHistograms) return; // canvas was closed
    if (GetUniqueID() == 0) SetLineWidth(GetLineWidth() + 2);
    SetUniqueID(10);
    gPad->Modified();
    gPad->Update();
    return;

  case kMouseLeave:
    if (GetUniqueID() == 10) SetLineWidth(GetLineWidth() - 2);
    SetUniqueID(0); // default
    gPad->Modified();
    gPad->Update();
    return;

  case kButton1Double:
    GetTracker()->ShowHistograms();
    return;
  }
}
//______________________________________________________________________________
char *TStrawTube::GetObjectInfo(Int_t px, Int_t py) const
{
  if (!gPad) return (char *)"";

  static char info[64];
  Double_t x = gPad->AbsPixeltoX(px);
  Double_t y = gPad->AbsPixeltoY(py);
  const char *xy = "X";
  if (strstr(GetTitle(), "y") || strstr(GetTitle(), "Y")) xy = "Y";
  sprintf(info, "Z=%7.2f, %s=%7.2f [cm]", gPad->PadtoX(x), xy, gPad->PadtoY(y));
  return info;
}
//______________________________________________________________________________
void TStrawTube::Print(Option_t *option) const
{
  Printf("%s: %s; tdc <%d, %d>", GetName(), GetTitle(), fTMin, fTMax);
}
//______________________________________________________________________________
const char *TStrawTube::GetName() const
{
  if (fNadc < 0) return "wrong";
  return Form("tube_%d", fNadc);
}
//______________________________________________________________________________
const char *TStrawTube::GetTitle() const
{
  if (!fLayer || !gVME) return (char *)"";
  return Form("%s %s[%d] %s", gVME->GetChannelInfo(fNadc),
              fLayer->GetName(), fLayer->GetNumb(), fLayer->GetTitle());
}
//______________________________________________________________________________
void TStrawTube::InitHistograms()
{
  // called(initialized) only once

  fhTime = new TH1F(Form("%s_time", GetName()),
                    Form("tdc   <%d, %d>; 10ns", fTMin, fTMax),
                    12000/30, 0, 12000);
  fhTime->SetMinimum(0);
  fhTime->SetLineColor(kGray + 1);
  gStrela->HistoManager(fhTime, "add");
  fhCutTime = new TH1F(Form("%s_cut", GetName()),
                       Form("tdc(cut)   <%d, %d>; 10ns", fCutT1, fCutT2),
                       12000/30, 0, 12000);
  fhCutTime->SetMinimum(0);
  gStrela->HistoManager(fhCutTime, "add");
  fhRadius = new TH1F(Form("%s_radius", GetName()), "radius; cm",
                      200*GetRange(), 0, GetRange());
  fhRadius->SetMinimum(0);
  gStrela->HistoManager(fhRadius, "add");
  fhDis1 = new TH1F(Form("%s_dis1", GetName()), "distance(1) from wire; cm",
                    50*GetRange(), -GetRange(), GetRange());
  gStrela->HistoManager(fhDis1, "add");
  fhDis2 = new TH1F(Form("%s_dis2", GetName()), "distance(2) from wire; cm",
                    50*GetRange(), -GetRange(), GetRange());
  fhDis2->SetLineColor(kGray + 1);
  gStrela->HistoManager(fhDis2, "add");
  fhEffi = new TH1F(Form("%s_effi", GetName()),
                    "efficiency; distance from wire, cm; efficiency",
                    50*GetRange(), -GetRange(), GetRange());
  fhEffi->Sumw2();
  gStrela->HistoManager(fhEffi, "add");

  fhTimeRes = new TH2F(Form("%s_time_res", GetName()),
                       "tdc : residual; tdc, 10ns; residual, cm",
                       100, 0, 5500, 100, -0.15, 0.15);
  fhTimeRes->GetYaxis()->CenterTitle();
  gStrela->HistoManager(fhTimeRes, "add");
  fhDisTime = new TH2F(Form("%s_dis_time", GetName()),
                       "distance from wire : tdc; distance from wire, cm; tdc, 10ns",
                       50*GetRange(), -1.1*GetRange(), 1.1*GetRange(),
                       100, 0, 5500);
  gStrela->HistoManager(fhDisTime, "add");
}
//______________________________________________________________________________
void TStrawTube::SetTime(Int_t t0, Int_t tmax)
{
  if (!fLayer) {
    Warning("SetTime", "tube %d without parent layer", fNadc);
    return;
  }

  fT0   = t0;
  fTMin = TMath::Min(t0, tmax);
  fTMax = TMath::Max(t0, tmax);

  if (!fhTime)
    InitHistograms();
  else
    fhTime->SetTitle(Form("tdc   <%d, %d>", fTMin, fTMax));
  if (fTMin == fTMax) {
    Warning("SetTime", "tube %d has Tmin = Tmax = %d", fNadc, fTMin);
    return;
  }

  fDriftVel = (GetRange() - fgWireRadius)/(fTMax - fTMin);
  // !!!
  // fDriftVel = (GetRange() - fgWireRadius)/4300; // !!! 4300 !!!
}
//______________________________________________________________________________
Double_t TStrawTube::T2R(Int_t time) const
{
  Double_t radius;
  if (fMulti && fMulti->GetSpline()) radius = fMulti->GetSpline()->Eval(time);
  else                               radius = fgWireRadius + fDriftVel*time;

  // !!! = 0 ???
  if (radius < fgWireRadius) radius = fgWireRadius;
  if (radius > GetRange()) radius = GetRange();
  fhRadius->Fill(radius);
  return radius;
}
//______________________________________________________________________________
void TStrawTube::SetShowHistograms(Option_t *option) const
{
  if (fgShowHistograms) return;

  fgShowHistograms = 1;
  Int_t ww = 610;
  TCanvas *c = new TCanvas("c_tube", "", gClient->GetDisplayWidth() - ww, 0,
                           ww, (UInt_t)(ww*1.20));
  TString opt = option;
  if (opt.Contains("cut", TString::kIgnoreCase))
    c->Divide(1, 2);
  else
    c->Divide(2, 3, 0.001, 0.001);
}
//______________________________________________________________________________
void TStrawTube::ShowHistograms() const
{
  TVirtualPad *save = gPad;
  TCanvas *c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("c_tube");
  if (!c) {
    fgShowHistograms = 0;
    return;
  }
  c->Clear("D"); // subpads are cleared but not deleted

  if (c->GetPad(3)) ShowHistoFull(c);
  else ShowHistoCut(c);

  c->SetTitle(GetTitle());
  c->Update();
  if (save) save->cd();
}
//______________________________________________________________________________
void TStrawTube::ShowHistoFull(TCanvas *can) const
{
  can->cd(1);
  fhTime->Draw();
  TLine line;
  line.SetLineColor(kBlue);
  line.DrawLine(fTMin, fhTime->GetMinimum(), fTMin, fhTime->GetMaximum());
  line.DrawLine(fTMax, fhTime->GetMinimum(), fTMax, fhTime->GetMaximum());
  fhCutTime->Draw("same");
  can->cd(2);
  fhRadius->Draw();
  can->cd(3);
  fhDis2->Draw();
  fhDis1->Draw("same");
  can->cd(4);
  fhTimeRes->Draw();
  can->cd(5);
  fhEffi->Draw();
  can->cd(6);
  fhDisTime->Draw();

  // tracker
  if (!gROOT->GetListOfCanvases()->FindObject("c_tracker")) return;
  static TStrawTracker *last = GetTracker();
  if (last == GetTracker()) return;
  last = GetTracker();
  last->ShowHistograms();
}
//______________________________________________________________________________
void TStrawTube::ShowHistoCut(TCanvas *can) const
{
  can->cd(1);
  fhTime->Draw();
  TLine line;
  line.SetLineColor(kRed);
  line.DrawLine(fCutT1, fhTime->GetMinimum(), fCutT1, fhTime->GetMaximum());
  line.DrawLine(fCutT2, fhTime->GetMinimum(), fCutT2, fhTime->GetMaximum());
  fhCutTime->Draw("same");
  can->cd(2);
  fhCutTime->Draw();
}
//______________________________________________________________________________
void TStrawTube::SetCutTime(Int_t t1, Int_t t2)
{
  fCutT1 = TMath::Min(t1, t2);
  fCutT2 = TMath::Max(t1, t2);

  if (fhCutTime)
    fhCutTime->SetTitle(Form("tdc(cut)   <%d, %d>", fCutT1, fCutT2));
  if (fCutT1 == fCutT2)
    Warning("SetCutTime", "tube %d has CutT1 = CutT2  = %d", fNadc, fCutT1);
}
