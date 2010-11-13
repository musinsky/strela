// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 25 Apr 2008

#include <TH2.h>
#include <TSpline.h>
#include <TROOT.h>
#include <TF1.h>
#include <TMath.h>
#include <TFile.h>

#include "TStrawMulti.h"
#include "TStrawTracker.h"

ClassImp(TStrawMulti)

//______________________________________________________________________________
TStrawMulti::TStrawMulti()
{
  //  Info("TStrawMulti", "Default constructor");
  fTubes    = 0;
  fSpline   = 0;
  fTimeMax  = 0;
  fRange    = 0;
  fhTimeRes = 0;
  fhTime    = 0;
  fCorrect  = 0;
}
//______________________________________________________________________________
TStrawMulti::TStrawMulti(const char *name, const char *title)
  : TNamed(name, title)
{
  //  Info("TStrawMulti", "Normal Constructor");
  fTubes    = 0;
  fSpline   = 0;
  fTimeMax  = 0;
  fRange    = 0;
  fhTimeRes = 0;
  fhTime    = 0;
  fCorrect  = 0;

  if (gStrela) gStrela->StrawCham()->Multies()->Add(this);
}
//______________________________________________________________________________
TStrawMulti::~TStrawMulti()
{
  Info("~TStrawMulti", "Destructor");
  if (fTubes)
    for (Int_t i = 0; i < fTubes->GetSize(); i++)
      ((TStrawTube *)fTubes->At(i))->SetMulti(0);
  delete fTubes; // tubes are not deleted
  delete fSpline;
  delete fhTimeRes; fhTimeRes = 0;
  delete fhTime;    fhTime = 0;
  delete fCorrect;
  if (gStrela) gStrela->StrawCham()->Multies()->Remove(this);
}
//______________________________________________________________________________
void TStrawMulti::Add(const TStrawLayer *layer)
{
  if (!layer) {
    Warning("Add", "%s, pointer to TStrawLayer is null", GetName());
    return;
  }

  AddAll(layer->Tubes());
}
//______________________________________________________________________________
void TStrawMulti::Add(const TStrawTracker *tracker)
{
  if (!tracker) {
    Warning("Add", "%s, pointer to TStrawTracker is null", GetName());
    return;
  }

  TIter next(gStrela->StrawCham()->Layers());
  TStrawLayer *layer;
  while((layer = (TStrawLayer *)next()))
    if (tracker == layer->GetTracker())
      Add(layer);
}
//______________________________________________________________________________
void TStrawMulti::AddAll(const TCollection *col)
{
  if (!fTubes) fTubes = new TList();

  TIter next(col);
  TStrawTube *tube;
  while ((tube = (TStrawTube *)next())) {
    if (!tube->InheritsFrom(TStrawTube::Class())) continue;
    TStrawMulti *prevMulti = tube->GetMulti();

    if (prevMulti == this) {
      Warning("AddAll", "%s already assigned to %s", tube->GetName(),
              GetName());
      continue;
    }
    else if (prevMulti) {
      Warning("AddAll", "%s change from %s to %s", tube->GetName(),
              prevMulti->GetName(), GetName());
      prevMulti->Tubes()->Remove(tube);
      tube->SetMulti(0);
    }

    if (fTubes->IsEmpty()) fRange = tube->GetRange(); // only first time
    if (fRange != tube->GetRange()) {
      Warning("AddAll", "%s from %s has other range", tube->GetName(),
              GetName());
      continue;
    }

    fTimeMax = TMath::Max(fTimeMax, tube->GetTMax() - tube->GetTMin());
    fTubes->Add(tube);
    tube->SetMulti(this);
  }
}
//______________________________________________________________________________
void TStrawMulti::SumTimeRes()
{
  if (!fTubes || fTubes->IsEmpty()) {
    if (fhTimeRes) delete fhTimeRes;
    fhTimeRes = 0;
    return;
  }

  if (!fhTimeRes) {
    fhTimeRes = new TH2F(*((TStrawTube *)fTubes->First())->HisTimeRes());
    fhTimeRes->SetName(Form("%s_time_res", GetName()));
    fhTimeRes->SetTitle(GetTitle());
    gStrela->HistoManager(fhTimeRes, "add");
  }

  fhTimeRes->Reset();
  TIter next(fTubes);
  TStrawTube *tube;
  while ((tube = (TStrawTube *)next()))
    fhTimeRes->Add(tube->HisTimeRes());
}
//______________________________________________________________________________
void TStrawMulti::SumTime()
{
  if (!HisTimeRes()) {
    if (fhTime) delete fhTime;
    fhTime = 0;
    return;
  }

  if (!fhTime) {
    // all projection histograms are deleted when file is closed
    fhTime = new TH1F(*((TH1F *)fhTimeRes->ProjectionX()));
    fhTime->SetName(Form("%s_time", GetName()));
    fhTime->SetTitle(GetTitle());
    gStrela->HistoManager(fhTime, "add");
    return;
  }

  fhTime->Reset();
  fhTime->Add(fhTimeRes->ProjectionX());
}
//______________________________________________________________________________
void TStrawMulti::IterFirst()
{
  if (!HisTime()) {
    Info("IterFirst", "%s, first fill histograms", GetName());
    return;
  }

  Int_t n = fhTime->GetNbinsX() + 1;
  Double_t x[n], y[n];
  x[0] = y[0] = 0.0;
  for (Int_t i = 1; i < n; i++) {
    x[i] = fhTime->GetBinCenter(i);
    y[i] = y[i-1] + fhTime->GetBinContent(i);
  }
  if (y[n-1] == 0) {
    if (fSpline) delete fSpline;
    fSpline = 0;
    Warning("IterFirst", "%s, empty histograms", GetName());
    return;
  }

  // last bin is also bin with maximum value
  Double_t rmin = TStrawTube::GetWireRadius();
  Double_t norm = (fRange - rmin)/y[n-1];
  for (Int_t i = 0; i < n; i++) y[i] = rmin + y[i]*norm;

  if (fSpline) delete fSpline;
  fSpline = new TSpline3(Form("%s_spline", GetName()), x, y, n);

  if (fCorrect) delete fCorrect;
  fCorrect = new TGraph();
  fCorrect->SetTitle("correction after autocalibration");
  fCorrect->SetMarkerStyle(8);
}
//______________________________________________________________________________
void TStrawMulti::IterNext(Int_t ndivide)
{
  // function similar as TH2::FitSliceY()

  Int_t iter = TStrawCham::GetIter();
  if (iter == 0) {
    IterFirst();
    return; // or IterFirst from linear
  }
  if (!HisTimeRes() || !fSpline) {
    Info("IterNext", "%s, first call IterFirst", GetName());
    return;
  }
  TF1 *g = (TF1 *)gROOT->GetFunction("gaus");
  if (!g) {
    Warning("IterNext", "unknown function gaus");
    return;
  }

  Int_t nbins = fhTimeRes->GetNbinsX();
  if (ndivide > nbins) ndivide = nbins;
  Double_t entries = fhTimeRes->GetEntries()/ndivide, sumdelta = 0.0, delta;
  Double_t *x = new Double_t[nbins+1];
  Double_t *y = new Double_t[nbins+1];
  Int_t i0 = 1, count = 0;
  x[count] = 0.0; y[count] = TStrawTube::GetWireRadius(); count++;

  for (Int_t i = i0; i <= nbins; i++) {
    // all projection histograms are deleted when file is closed
    TH1D *proj = fhTimeRes->ProjectionY("_py", i0, i);
    if ((proj->GetEntries() < entries) && (i != nbins)) continue;

    if (count >= (nbins + 1)) {
      Warning("IterNext", "%s, to many number of knots", GetName());
      break;
    }

    delta = proj->GetBinCenter(proj->GetMaximumBin());
    g->SetParameters(1, 0, 1); // default gaus
    proj->Fit(g, "QN", "", delta - proj->GetRMS(), delta + proj->GetRMS());
    if (TMath::Abs(g->GetParameter(1) - delta) > (fRange/50.0)) {
      Warning("IterNext", "%s, problem wih fitting", GetName());
      continue;
    }
    else delta = g->GetParameter(1); // mean of gaus

    x[count]  = (fhTimeRes->GetBinCenter(i0) + fhTimeRes->GetBinCenter(i))/2.0;
    y[count]  = fSpline->Eval(x[count]) - delta;
    if (y[count] > fRange)     y[count] = fRange;
    if (y[count] < y[count-1]) y[count] = y[count-1]; // must be increasing
    if (gDebug > 0)
      Printf("[%3d, %3d]; x = %6.1f, y = %5.3f", i0, i, x[count], y[count]);
    sumdelta += delta*delta;
    i0 = i + 1;
    count++;
  }

  if (fSpline) delete fSpline;
  // option "e1/e2" give better result on border of spline, need investigate
  fSpline = new TSpline3(Form("%s_spline", GetName()), x, y, count, "e1");
  fCorrect->SetPoint(iter - 1, iter, TMath::Sqrt(sumdelta/count));

  delete [] x;
  delete [] y;
}
//______________________________________________________________________________
void TStrawMulti::ReadWriteSpline(TFile *file)
{
  if (!file) return;
  TString sname = Form("%s_spline", GetName());

  // write
  if (file->IsWritable()) {
    if (!fSpline) return;
    TDirectory *dir = gDirectory;
    file->cd();
    fSpline->Write(sname.Data(), TObject::kOverwrite);
    Printf("%s was written to %s", sname.Data(), file->GetName());
    dir->cd();
    return;
  }

  // read
  if (!file->FindKey(sname.Data())) return;
  if (fSpline) delete fSpline;
  file->GetObject(sname.Data(), fSpline);
  if (fSpline) Printf("%s was read from %s", sname.Data(), file->GetName());
}
