// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    10 Feb 2015

#include <TH2.h>
#include <TSpline.h>
#include <TROOT.h>
#include <TF1.h>
#include <TMath.h>
#include <TFile.h>
#include <Varargs.h>

#include "TStrawMulti.h"
#include "TStrawTracker.h"

ClassImp(TStrawMulti)

//______________________________________________________________________________
TStrawMulti::TStrawMulti()
{
  //  Info("TStrawMulti", "Default constructor");
  fTubes    = 0;
  fSpline   = 0;
  fRange    = 0;
  fhTimeRes = 0;
  fCorrect  = 0;
}
//______________________________________________________________________________
TStrawMulti::TStrawMulti(const char *name, const char *title) : TNamed(name, title)
{
  //  Info("TStrawMulti", "Normal Constructor");
  fTubes    = 0;
  fSpline   = 0;
  fRange    = 0;
  fhTimeRes = 0;
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
  delete fCorrect;
  if (gStrela) gStrela->StrawCham()->Multies()->Remove(this);
}
//______________________________________________________________________________
void TStrawMulti::AddAnything(TObject *va_(obj1), ...)
{
  //  list of trackers/layers/tubes must be temrinated by 0
  if (!va_(obj1)) return;

  va_list ap;
  va_start(ap, va_(obj1));
  TObject *obj = va_(obj1);

  do {
    if (obj->InheritsFrom(TStrawTracker::Class()))
      Add((TStrawTracker *)obj);
    else if (obj->InheritsFrom(TStrawLayer::Class()))
      Add((TStrawLayer *)obj);
    else if (obj->InheritsFrom(TStrawTube::Class()))
      Add((TStrawTube *)obj);
    else
      Warning("AddAnything", "add %s is not possible", obj->GetName());
  } while ((obj = va_arg(ap, TObject *)));

  va_end(ap);
}
//______________________________________________________________________________
void TStrawMulti::Add(const TStrawTracker *tracker)
{
  if (!tracker) {
    Warning("Add", "%s, pointer to TStrawTracker is null", GetName());
    return;
  }

  TIter next(tracker->Layers());
  TStrawLayer *layer;
  while ((layer = (TStrawLayer *)next()))
    Add(layer);
}
//______________________________________________________________________________
void TStrawMulti::Add(const TStrawLayer *layer)
{
  if (!layer) {
    Warning("Add", "%s, pointer to TStrawLayer is null", GetName());
    return;
  }

  TIter next(layer->Tubes());
  TStrawTube *tube;
  while ((tube = (TStrawTube *)next()))
    Add(tube);
}
//______________________________________________________________________________
void TStrawMulti::Add(TStrawTube *tube)
{
  if (!tube) {
    Warning("Add", "%s, pointer to TStrawTube is null", GetName());
    return;
  }

  TStrawMulti *prevMulti = tube->GetMulti();
  if (prevMulti == this) {
    Warning("Add", "%s already assigned to %s", tube->GetName(), GetName());
    return;
  }
  else if (prevMulti) {
    Warning("Add", "%s change from %s to %s", tube->GetName(), prevMulti->GetName(), GetName());
    prevMulti->RemoveTube(tube);
  }

  if (!fTubes) fRange = tube->GetRange(); // only once
  if (fRange != tube->GetRange()) {
    Warning("Add", "%s from %s has another range", tube->GetName(), GetName());
    return;
  }

  if (!fTubes) fTubes = new TList();
  fTubes->Add(tube);
  tube->SetMulti(this);
}
//______________________________________________________________________________
void TStrawMulti::RemoveTube(TStrawTube *tube)
{
  if (!tube) {
    Warning("RemoveTube", "%s, pointer to TStrawTube is null", GetName());
    return;
  }

  if (fTubes) {
    if (tube == fTubes->Remove(tube)) tube->SetMulti(0);
    else Warning("RemoveTube", "%s is not assigned to %s", tube->GetName(), GetName());
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

  if (!fhTimeRes) { // create histo frame (only once)
    fhTimeRes = new TH2F(*((TStrawTube *)fTubes->First())->HisTimeRes());
    fhTimeRes->SetName(Form("%s_time_res", GetName()));
    gStrela->HistoManager(fhTimeRes, "add");
  }

  fhTimeRes->Reset();
  TIter next(fTubes);
  TStrawTube *tube;
  while ((tube = (TStrawTube *)next()))
    if (!tube->IsNoMultiSum()) fhTimeRes->Add(tube->HisTimeRes());
}
//______________________________________________________________________________
void TStrawMulti::IterFirst()
{
  if (!HisTimeRes()) {
    Warning("IterFirst", "%s without any tubes", GetName());
    return;
  }

  Int_t nb = fhTimeRes->GetNbinsX();
  // all projection histograms are deleted when file is closed
  TH1D *htemp = fhTimeRes->ProjectionX("_px", 1, nb);
  Double_t *x = new Double_t[nb];
  Double_t *y = new Double_t[nb];
  Double_t sum = 0;
  for (Int_t i = 0; i < nb; i++) {
    x[i] = htemp->GetBinCenter(i+1);
    sum += htemp->GetBinContent(i+1);
    y[i] = sum;
  }

  if (sum == 0) {
    if (fSpline) delete fSpline;
    fSpline = 0;
    Warning("IterFirst", "%s, first analyze entries", GetName());
    delete [] x;
    delete [] y;
    return;
  }

  Double_t rmin = TStrawTube::GetWireRadius();
  Double_t norm = (fRange - rmin)/sum;
  for (Int_t i = 0; i < nb; i++) y[i] = rmin + y[i]*norm;

  if (fSpline) delete fSpline;
  fSpline = new TSpline3(Form("%s_spline", GetName()), x, y, nb);
  // need checking grows full time interval
  delete [] x;
  delete [] y;

  if (fCorrect) delete fCorrect;
  fCorrect = new TGraph();
  fCorrect->SetTitle(Form("%s correction after autocalibration", GetName()));
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
    if (gDebug > 0) Printf("[%3d, %3d]; x = %6.1f, y = %5.3f", i0, i, x[count], y[count]);
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
