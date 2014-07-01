// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    01 Jul 2014

#include <TH2.h>
#include <TSpline.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TGClient.h>
#include <TTimer.h>
#include <TF1.h>

#include "TStrawTube.h"
#include "TStrawTracker.h"
#include "TStrawMulti.h"
#include "TVME.h"
#include "TStrelaDisplay.h"

Int_t    TStrawTube::fgBaseT0         = 0;
Double_t TStrawTube::fgDriftVel       = 2.1/4400;
Double_t TStrawTube::fgWireRadius     = 0.0025/2;
Int_t    TStrawTube::fgOnlyFirstNHits = 5;
Int_t    TStrawTube::fgShowHistograms = 0;

const Int_t kBaseT0 = TStrawTube::GetBaseT0();

ClassImp(TStrawTube)

//______________________________________________________________________________
TStrawTube::TStrawTube()
{
  //  Info("TStrawTube", "Default constructor");
  fCenter   = 0;
  fT0       = 0;
  fTMin     = 0;
  fTMax     = 0;
  fNadc     = -1;
  fLayer    = 0;
  fMulti    = 0;
  fMargin   = kNoMargin;
  fNHits    = 0;
  fhTime1   = 0;
  fhTime2   = 0;
  fhRad1    = 0;
  fhRad2    = 0;
  fhDis1    = 0;
  fhDis2    = 0;
  fhEffi    = 0;
  fhTimeRes = 0;
  fhDisTime = 0;
  fhBzRes   = 0;
  fhBzAz    = 0;
}
//______________________________________________________________________________
TStrawTube::TStrawTube(Double_t center) : TEllipse()
{
  //  Info("TStrawTube", "Normal constructor");
  fCenter  = center;
  fT0       = 0;
  fTMin     = 0;
  fTMax     = 0;
  fNadc     = -1;
  fLayer    = 0;
  fMulti    = 0;
  fMargin   = kNoMargin;
  fNHits    = 0;
  fhTime1   = 0;
  fhTime2   = 0;
  fhRad1    = 0;
  fhRad2    = 0;
  fhDis1    = 0;
  fhDis2    = 0;
  fhEffi    = 0;
  fhTimeRes = 0;
  fhDisTime = 0;
  fhBzRes   = 0;
  fhBzAz    = 0;
}
//______________________________________________________________________________
TStrawTube::~TStrawTube()
{
  //  Info("~TStrawTube", "Destructor");
  delete fhTime1;   fhTime1 = 0;
  delete fhTime2;   fhTime2 = 0;
  delete fhRad1;    fhRad1 = 0;
  delete fhRad2;    fhRad2 = 0;
  delete fhDis1;    fhDis1 = 0;
  delete fhDis2;    fhDis2 = 0;
  delete fhEffi;    fhEffi = 0;
  delete fhTimeRes; fhTimeRes = 0;
  delete fhDisTime; fhDisTime = 0;
  delete fhBzRes;   fhBzRes = 0;
  delete fhBzAz;    fhBzAz = 0;
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
void TStrawTube::ExecuteEvent(Int_t event, Int_t /*px*/, Int_t /*py*/)
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
      TCanvas *c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("c_tracker");
      if (c) c->Close();
      else GetTracker()->ShowHistograms();
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
void TStrawTube::Print(Option_t * /*option*/) const
{
  Printf("%s: %s; T0 = %4d, tdc <%4d, %4d>", GetName(), GetTitle(), fT0, fTMin, fTMax);
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
void TStrawTube::SetDisabled(Bool_t set)
{
  if (IsDisabled() == set) return;
  SetBit(kIsDisabled, set);
  if (set) SetLineColor(GetLineColor() + 17); // gray18
  else     SetLineColor(GetLineColor() - 17);
  if (gStrela->GetDisplay()) gStrela->GetDisplay()->FindPad(this);
}
//______________________________________________________________________________
void TStrawTube::SetNoMultiSum(Bool_t set)
{
  if (IsNoMultiSum() == set) return;
  SetBit(kNoMultiSum, set);
  if (set) SetLineColor(GetLineColor() + 5); // magenta
  else     SetLineColor(GetLineColor() - 5);
  if (gStrela->GetDisplay()) gStrela->GetDisplay()->FindPad(this);
}
//______________________________________________________________________________
void TStrawTube::HighLight(Bool_t flash, Int_t ms)
{
  if (flash) {
    SetLineColor(GetLineColor() + 3); // blue
    SetLineWidth(GetLineWidth() + 2);
    TTimer::SingleShot(ms, IsA()->GetName(), this, "HighLight(=kFALSE)");
  }
  else {
    SetLineColor(GetLineColor() - 3);
    SetLineWidth(GetLineWidth() - 2);
  }
  if (gStrela->GetDisplay()) gStrela->GetDisplay()->FindPad(this);
}
//______________________________________________________________________________
void TStrawTube::InitHistograms()
{
  // called(initialized) only once, not in the constructor
  if (!fLayer) {
    Warning("InitHistograms", "tube %d without parent layer", fNadc);
    return;
  }

  fhTime1 = new TH1F(Form("%s_time1", GetName()), Form("T0 = %d, tdc   <%d, %d>; 10ns", fT0, fTMin, fTMax),
                     9000/30, 0, 9000);
  fhTime1->SetMinimum(0);
  fhTime1->SetLineColor(kGray + 1);
  gStrela->HistoManager(fhTime1, "add");
  fhTime2 = new TH1F(Form("%s_time2", GetName()), Form("tdc   <%d, %d>; 10ns", fTMin, fTMax),
                     9000/30, 0, 9000);
  fhTime2->SetMinimum(0);
  gStrela->HistoManager(fhTime2, "add");
  fhRad1 = new TH1F(Form("%s_rad1", GetName()), "radius(1); cm", 200*GetRange(), 0, GetRange());
  fhRad1->SetMinimum(0);
  fhRad1->SetLineColor(kGray + 1);
  gStrela->HistoManager(fhRad1, "add");
  fhRad2 = new TH1F(Form("%s_rad2", GetName()), "radius(2); cm", 200*GetRange(), 0, GetRange());
  fhRad2->SetMinimum(0);
  gStrela->HistoManager(fhRad2, "add");
  fhDis1 = new TH1F(Form("%s_dis1", GetName()), "distance(1) from wire; cm",
                    50*GetRange(), -GetRange(), GetRange());
  fhDis1->SetMinimum(0);
  gStrela->HistoManager(fhDis1, "add");
  fhDis2 = new TH1F(Form("%s_dis2", GetName()), "distance(2) from wire; cm",
                    50*GetRange(), -GetRange(), GetRange());
  fhDis2->SetMinimum(0);
  fhDis2->SetLineColor(kGray + 1);
  gStrela->HistoManager(fhDis2, "add");
  fhEffi = new TH1F(Form("%s_effi", GetName()), "efficiency; distance from wire, cm; efficiency",
                    50*GetRange(), -GetRange(), GetRange());
  fhEffi->Sumw2();
  fhEffi->SetLineColor(kCyan);
  gStrela->HistoManager(fhEffi, "add");

  fhTimeRes = new TH2F(Form("%s_time_res", GetName()), "tdc : residual; tdc, 10ns; residual, cm",
                       200, kBaseT0-500, kBaseT0+5500, 100, -0.15, 0.15);
  fhTimeRes->GetYaxis()->CenterTitle();
  gStrela->HistoManager(fhTimeRes, "add");
  fhDisTime = new TH2F(Form("%s_dis_time", GetName()), "distance from wire : tdc; dist., cm; tdc, 10ns",
                       50*GetRange(), -1.1*GetRange(), 1.1*GetRange(), 100, kBaseT0-500, kBaseT0+5500);
  fhDisTime->GetYaxis()->CenterTitle();
  gStrela->HistoManager(fhDisTime, "add");
  fhBzRes = new TH2F(Form("%s_bz_res", GetName()), "intercept : residual; intercept, cm; residual, cm",
                     50*GetRange(), -1.1*GetRange(), 1.1*GetRange(), 100, -0.15, 0.15);
  fhBzRes->GetYaxis()->CenterTitle();
  gStrela->HistoManager(fhBzRes, "add");
  fhBzAz = new TH2F(Form("%s_bz_az", GetName()), "intercept : slope; intercept, cm; slope, rad",
                    50*GetRange(), -1.1*GetRange(), 1.1*GetRange(), 100, -0.1, 0.1);
  fhBzAz->GetYaxis()->CenterTitle();
  gStrela->HistoManager(fhBzAz, "add");
}
//______________________________________________________________________________
void TStrawTube::SetTMinMax(Int_t tmin, Int_t tmax, Bool_t delta)
{
  if (!delta) {
    fTMin = tmin;
    fTMax = tmax;
  }
  else {
    fTMin += tmin;
    fTMax += tmax;
  }

  TimesChanged();
  //  if (fTMin == fTMax) Warning("SetTMinMax", "tube %d has Tmin = Tmax = %d", fNadc, fTMin);
}
//______________________________________________________________________________
void TStrawTube::TimesChanged()
{
  if (!fhTime1)
    InitHistograms(); // only once
  else {
    fhTime1->SetTitle(Form("T0 = %d, tdc   <%d, %d>", fT0, fTMin, fTMax));
    fhTime2->SetTitle(Form("tdc   <%d, %d>", fTMin, fTMax));
  }
}
//______________________________________________________________________________
Double_t TStrawTube::T2R(Int_t time) const
{
  Double_t radius;
  if (fMulti && fMulti->GetSpline()) radius = fMulti->GetSpline()->Eval(time);
  else radius = fgWireRadius + fgDriftVel*(time - kBaseT0);

  if (radius < fgWireRadius) radius = fgWireRadius;
  if (radius > GetRange()) radius = GetRange();
  fhRad1->Fill(radius);
  return radius;
}
//______________________________________________________________________________
Int_t TStrawTube::R2T(Double_t radius) const
{
  // need for Monte Carlo

  radius = TMath::Abs(radius);
  if      (radius < fgWireRadius) return (TInT0(fTMin) - 500);
  else if (radius > GetRange())   return (TInT0(fTMax) + 500);
  else                            return (radius/fgDriftVel);
}
//______________________________________________________________________________
void TStrawTube::SetShowHistograms(Option_t *option) const
{
  if (fgShowHistograms) return;

  fgShowHistograms = 1;
  Int_t ww = 610;
  TCanvas *c = new TCanvas("c_tube", "", gClient->GetDisplayWidth() - ww, 0, ww, ww*1.20);
  TString opt = option;
  if (opt.Contains("time", TString::kIgnoreCase))
    c->Divide(1, 2);
  else {
    c->Divide(2, 3, 0.001, 0.001);
    c->GetPad(3)->SetGrid();
    c->GetPad(4)->SetGrid();
  }
}
//______________________________________________________________________________
void TStrawTube::ShowHistograms(Option_t * /*option*/) const
{
  TVirtualPad *save = gPad;
  TCanvas *c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("c_tube");
  if (!c) {
    fgShowHistograms = 0;
    return;
  }
  c->Clear("D"); // subpads are cleared but not deleted

  if (c->GetPad(3)) ShowHistoFull(c);
  else ShowHistoTime(c);

  c->SetTitle(GetTitle());
  c->Update();
  if (save) save->cd();
}
//______________________________________________________________________________
void TStrawTube::ShowHistoFull(TCanvas *can) const
{
  can->cd(1);
  fhTime1->Draw();
  TLine line;
  line.SetLineColor(kBlack);
  line.SetLineStyle(3);
  line.DrawLine(fTMin, fhTime1->GetMinimum(), fTMin, fhTime1->GetMaximum());
  line.DrawLine(fTMax, fhTime1->GetMinimum(), fTMax, fhTime1->GetMaximum());
  line.SetLineColor(kGreen);
  line.SetLineStyle(2);
  line.DrawLine(TExT0(0), fhTime1->GetMinimum(), TExT0(0), fhTime1->GetMaximum());
  fhTime2->Draw("same");
  can->cd(2);
  fhRad1->Draw();
  fhRad2->Draw("same");
  can->cd(3);
  fhTimeRes->Draw("COL");
  line.DrawLine(kBaseT0, fhTimeRes->GetYaxis()->GetXmin(), kBaseT0, fhTimeRes->GetYaxis()->GetXmax());
  can->cd(4);
  fhBzRes->Draw("COL");
  can->cd(5);
  fhBzAz->Draw("COL");
  //  fhDis2->Draw();
  //  fhDis1->Draw("same");
  //  Double_t max = fhDis2->GetMaximum()/2.0, maxe = fhEffi->GetMaximum();
  //  if ((Int_t) max != (Int_t) maxe) fhEffi->Scale(max/maxe);
  //  fhEffi->Draw("same");
  can->cd(6);
  fhDisTime->Draw("COL");
  //  fhBzAz->Draw("COL");

  // tracker
  if (!gROOT->GetListOfCanvases()->FindObject("c_tracker")) return;
  static TStrawTracker *last = GetTracker();
  if (last == GetTracker()) return;
  last = GetTracker();
  last->ShowHistograms();
}
//______________________________________________________________________________
void TStrawTube::ShowHistoTime(TCanvas *can) const
{
  can->cd(1);
  fhTime1->Draw();
  TLine line;
  line.SetLineColor(kBlack);
  line.SetLineStyle(3);
  line.DrawLine(fTMin, fhTime1->GetMinimum(), fTMin, fhTime1->GetMaximum());
  line.DrawLine(fTMax, fhTime1->GetMinimum(), fTMax, fhTime1->GetMaximum());
  line.SetLineColor(kGreen);
  line.SetLineStyle(2);
  line.DrawLine(fT0, fhTime1->GetMinimum(), fT0, fhTime1->GetMaximum());
  fhTime2->Draw("same");
  can->cd(2);
  fhTime2->Draw();
}
//______________________________________________________________________________
void TStrawTube::AlignCenter()
{
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // TODO
  Int_t fb = fhBzRes->GetXaxis()->FindBin(-2.1*0.9);
  Int_t lb = fhBzRes->GetXaxis()->FindBin(-2.1*0.1);
  TH1D *proj = fhBzRes->ProjectionY("_py", fb, lb);
  //  Double_t en = proj->GetEntries();

  TF1 *g = (TF1 *)gROOT->GetFunction("gaus");
  if (!g) {
    Warning("IterNext", "unknown function gaus");
    return;
  }
  proj->Fit(g, "QN");
  Double_t delMinus = g->GetParameter(1);
  if (g->GetParameter(2) > 0.05) {
    Printf("Problem minus");
    return;
  }
  g->SetParameters(1, 0, 1); // default gaus

  fb = fhBzRes->GetXaxis()->FindBin(2.1*0.1);
  lb = fhBzRes->GetXaxis()->FindBin(2.1*0.9);
  proj = fhBzRes->ProjectionY("_py", fb, lb);
  //  if (TMath::Abs(en - proj->GetEntries())/en < 0.25) {
  //    Printf("velky rozdiel v L/R entries");
  //    return;
  //  }

  proj->Fit(g, "QN");
  Double_t delPlus = g->GetParameter(1);
  if (g->GetParameter(2) > 0.05) {
    Printf("Problem plus");
    return;
  }

  Double_t delta = (delPlus-delMinus)/2.0; // len polovicu !!!!!
  // !!!!!!!!!!
  //  Printf("%s pred  %7.3f", GetName(), GetCenter());
  SetCenter(GetCenter() - delta);
  //  Printf("%s potom %7.3f", GetName(), GetCenter());
  Printf("%s rozdiel %5.3f",  GetName(), delta);
}
//______________________________________________________________________________
void TStrawTube::AlignCenterTime()
{
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // TODO
  TH1D *proj = fhTimeRes->ProjectionY();
  TF1 *g = (TF1 *)gROOT->GetFunction("gaus");
  if (!g) {
    Warning("IterNext", "unknown function gaus");
    return;
  }
  proj->Fit(g, "QN");
  //  Double_t mean = g->GetParameter(1);
  //  !!! zmena SetT0 !!!
  //  Printf("%s pred  %d", GetName(), fTMin);
  //  SetTTTime_TMinMax(fTMin + (mean/fgDriftVel), fTMax);
  //  Printf("%s potom %d", GetName(), fTMin);
}
