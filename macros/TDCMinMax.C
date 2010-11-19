// root[] .x Strela.C
// root[] .L macros/TDCMinMax.C+
// root[] gStrela->AnalyzeEntries()
// root[] TDCMinMax(); > seans_channels_updateTDC.sql

// Author: Jan Musinsky
// 19/11/2010

#include <TH1.h>
#include <TF1.h>
#include <TROOT.h>
#include <TFile.h>

#include "TStrela.h"
#include "TStrawCham.h"
#include "TStrawTube.h"

// hint: run with dt0 = dt1 = -9999, force common time interval
// const Int_t t0 = 3930, t1 = 8880; // common time interval, 2007_03
const Int_t t0 = 950, t1 = 5800; // common time interval, 2009_12
const Double_t dt0 = (t1 - t0)*0.03, dt1 = (t1 - t0)*0.08;

Int_t FindTime(TH1 *histo, Option_t *opt = "", Bool_t eol = kTRUE)
{
  if (!histo) return 0;

  Double_t xmin = histo->GetXaxis()->GetXmin();
  Double_t xmax = histo->GetXaxis()->GetXmax();
  Double_t mean = histo->GetMean();
  Double_t rms  = histo->GetRMS();
  TF1 *fun = (TF1 *)gROOT->GetFunction("fun");
  if (!fun)
    fun = new TF1("fun", "[2]+[3]*TMath::Erfc((x-[0])/([1]*TMath::Sqrt2()))");
  fun->SetParameters(0, 0, 0, 0);
  Double_t tmin, tmax, p0, p1;
  TString fopt  = opt;

  // tmin: slope / => p1 > 0 (p1 ~ 20)
  fun->SetParameter(0, mean - rms);
  Double_t p1min = 5;
  Double_t p1max = 100;
  fun->SetParLimits(1, p1min, p1max);
  fun->SetLineColor(kRed);
  histo->Fit(fun, fopt, "", (xmin + (mean - rms))/2.0, mean);
  p0 = fun->GetParameter(0);
  p1 = fun->GetParameter(1);
  // or better fitting with "E"
  if (TMath::Abs(p1 - p1min) < 0.1) p1*=10.0; // increasing fit interval
  if (TMath::Abs(p1 - p1max) < 0.1) p1/=2.0;  // decreasing fit interval
  fun->SetLineColor(kGreen);
  histo->Fit(fun, fopt+"+", "", p0 - 20*p1, p0 + 20*p1);
  p0 = fun->GetParameter(0);
  p1 = fun->GetParameter(1);
  tmin = p0 - 1.00*p1;

  // tmax: slope \ => p1 < 0 (p1 ~ -300)
  fun->SetParameter(0, mean + rms);
  fun->SetParLimits(1, -1000, -50);
  fun->SetLineColor(kRed - 9);
  histo->Fit(fun, fopt+"+", "", mean, (xmax + (mean + rms))/2.0);
  p0 = fun->GetParameter(0);
  p1 = fun->GetParameter(1);
  fun->SetLineColor(kGreen - 9);
  histo->Fit(fun, fopt+"+", "", p0 + 2*p1, p0 - 2*p1);
  p0 = fun->GetParameter(0);
  p1 = fun->GetParameter(1);
  tmax = p0 - 1.00*p1;

  // checking time interval
  Int_t check = 0;
  if (((t0 - dt0) > tmin) || ((t0 + dt0) < tmin)) { tmin = t0; check += 1; }
  if (((t1 - dt1) > tmax) || ((t1 + dt1) < tmax)) { tmax = t1; check += 2; }
  printf("T0 = %5d, TMax = %5d", (Int_t)tmin, (Int_t)tmax);
  if (eol) printf("\n");
  else return check;

  TH1F *histoc = (TH1F*)histo->Clone(Form("c%s", histo->GetName()));
  histoc->SetBit(kCanDelete);
  for (Int_t i = 1; i <= histo->GetNbinsX(); i++)
    if ((histo->GetBinCenter(i) < tmin) || (histo->GetBinCenter(i) > tmax))
      histoc->SetBinContent(i, 0);
  histoc->SetFillColor(kBlue - 10);
  histoc->Draw("same");
  return check;
}

void TDCMinMax(const char *fname = 0)
{
  if (!gStrela->StrawCham()) return;

  TFile *hfile = 0;
  if (fname) hfile = TFile::Open(fname);

  TIter next(gStrela->StrawCham()->Tubes());
  TStrawTube *tube;
  while ((tube = (TStrawTube *)next())) {
    if (hfile) {
      TH1F *fhis;
      hfile->GetObject(tube->HisTime1()->GetName(), fhis);
      if (!fhis) Printf("%s does not exist", tube->HisTime1()->GetName());
      else {
        tube->HisTime1()->Reset();
        tube->HisTime1()->Add(fhis);
        tube->HisTime1()->SetTitle(fhis->GetTitle());
      }
    }

    printf("UPDATE %s.channels SET ", gStrela->GetSeance());
    Int_t check = FindTime(tube->HisTime1(), "Q", kFALSE);
    printf(" WHERE channels.Nadc = %5d LIMIT 1;", tube->GetNadc());
    if (check == 3)      printf(" /* check T0, TMax */");
    else if (check == 2) printf(" /* check TMax */");
    else if (check == 1) printf(" /* check T0 */");
    printf("\n");
  }
  delete hfile;
}
