// Author: Jan Musinsky
// 19/11/2010

/*
  .x Strela.C
  .L macros/TDCT0.C+
  TStrawCham::Tracking(-1)
  TStrawTube::OnlyFirstNHits(1)
  gStrela->StrawCham()->SetTubesTimes(0, 0, 9000)
  gStrela->StrawCham()->GetTube(0)->SetShowHistograms("time")
  gStrela->AnalyzeEntries()
  TDCT0(); > seans_YYYY_MM_channels_updateT0.sql
 */

#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TBox.h>
#include <TROOT.h>
#include <TFile.h>

#include "TStrela.h"
#include "TStrawCham.h"
#include "TStrawTracker.h"
#include "TStrawTube.h"

const Double_t p0_mean = 2000, p0_pm = 1500;
const Double_t p1_mean = 25, p1_min = 5, p1_max = 100;

void FindT0(TStrawTube *tube, Double_t p1coef, Option_t *opt = "")
{
  if (!tube) return;
  TCanvas *c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("c_tube_fit");
  if (!c) c = new TCanvas("c_tube_fit", "c_tube_fit");
  c->cd();

  Double_t p0, p1;
  TH1F *histo = tube->HisTime1();
  TF1 *fun = (TF1 *)gROOT->GetFunction("fun");
  if (!fun)
    //    fun = new TF1("fun", "[2]+[3]*TMath::Erfc((x-[0])/([1]*TMath::Sqrt2()))"); // OK je to sigma gausa
    fun = new TF1("fun", "[2]+[3]*TMath::Erfc((x-[0])/[1])");
  fun->SetParameters(0, 0, 0, 0);

  // t0 time: slope / => p1 > 0 (p1 ~ 25)
  // t0 interval: (-2*p1, p0, 2*p1) => width of increasing interval ~ 4*p1
  fun->SetParameter(0, p0_mean);
  fun->SetParameter(1, p1_mean);
  fun->SetParLimits(1, p1_min, p1_max);
  fun->SetLineColor(kRed); fun->SetLineWidth(1); fun->SetLineStyle(3);
  histo->Fit(fun, opt, "", p0_mean - p0_pm, p0_mean + p0_pm);
  p0 = fun->GetParameter(0);
  p1 = fun->GetParameter(1);
  fun->SetLineColor(kViolet); fun->SetLineWidth(1); fun->SetLineStyle(2);
  histo->Fit(fun, Form("%s+", opt), "", p0 - 10*p1, p0 + 10*p1);
  p0 = fun->GetParameter(0);
  p1 = fun->GetParameter(1);
  fun->SetLineColor(kGreen); fun->SetLineWidth(1); fun->SetLineStyle(1);
  histo->Fit(fun, Form("%s+", opt), "", p0 - 10*p1, p0 + 10*p1);
  p0 = fun->GetParameter(0);
  p1 = fun->GetParameter(1);
  Int_t t0 = p0 - p1coef*p1 + 0.5, t0sig = p1 + 0.5;

  printf("UPDATE `%s`.`channels` SET ", gStrela->GetSeance());
  printf("`T0` = '%5d' /* %3d */ ", t0, t0sig);
  printf("WHERE `channels`.`Nadc` = %5d;\n", tube->GetNadc());

  histo->SetTitle(Form("tdc   <%d, %d>, T0 = %4.0f - %3.2f*%3.0f = %d",
                       tube->GetTMin(), tube->GetTMax(), p0, p1coef, p1, t0));
  histo->GetXaxis()->SetRangeUser(p0_mean - p0_pm, p0_mean + p0_pm);
  Double_t ymin = histo->GetMinimum(), ymax = histo->GetMaximum();
  TLine *line = (TLine *)histo->FindObject("TLine");
  if (!line) {
    line = new TLine(t0, ymin, t0, ymax);
    line->SetLineColor(kBlue);
    line->SetLineWidth(2);
    histo->GetListOfFunctions()->Add(line);
  }
  else {
    line->SetX1(t0); line->SetY1(ymin);
    line->SetX2(t0); line->SetY2(ymax);
  }
  TBox *box = (TBox *)histo->FindObject("TBox");
  if(!box) {
    box = new TBox(p0 - p1coef*p1, ymin, p0 + p1coef*p1, ymax);
    box->SetFillStyle(3001); box->SetFillColor(kGray+2);
    histo->GetListOfFunctions()->Add(box);
  }
  else {
    box->SetX1(p0 - p1coef*p1); box->SetY1(ymin);
    box->SetX2(p0 + p1coef*p1); box->SetY2(ymax);
  }

  histo->GetXaxis()->UnZoom();
  TH1F *histo2 = tube->HisTime2();
  histo2->Scale(histo->GetMaximum()/histo2->GetMaximum());

  delete histo2->GetListOfFunctions()->Remove(histo2->FindObject("h_clone"));
  TH1F *h_clone = (TH1F *)histo->Clone("h_clone");
  h_clone->SetDirectory(0);
  histo2->GetListOfFunctions()->Add(h_clone, "same");

  histo->GetXaxis()->SetRangeUser(p0_mean - p0_pm, p0_mean + p0_pm);
}

void ReplaceHistoFromFile(TH1 *histo, TFile *file)
{
  if (!histo || !file) return;

  TH1 *fhis;
  file->GetObject(histo->GetName(), fhis);
  if (!fhis) Printf("%s does not exist", histo->GetName());
  else {
    histo->Reset();
    histo->Add(fhis);
    histo->SetTitle(fhis->GetTitle());
  }
}

void TDCT0(const char *fname = 0, Double_t coef = 1.75)
{
  if (!gStrela->StrawCham()) return;

  TFile *hfile = 0;
  if (fname) hfile = TFile::Open(fname);
  TStrawTracker *tracker;
  TStrawLayer *layer;
  TStrawTube *tube;
  for (Int_t i = 0; i < gStrela->StrawCham()->Trackers()->GetSize(); i++) {
    tracker = (TStrawTracker *)gStrela->StrawCham()->Trackers()->At(i);
    for (Int_t j = 0; j < tracker->Layers()->GetSize(); j++) {
      layer = tracker->GetLayer(j);
      printf("/*%10s*/\n", layer->GetName());
      for (Int_t k = 0; k < layer->Tubes()->GetSize(); k++) {
        tube = layer->GetTube(k);
        if (hfile) {
          ReplaceHistoFromFile(tube->HisTime1(), hfile);
          ReplaceHistoFromFile(tube->HisTime2(), hfile);
        }
        tube->HisTime2()->Scale(tube->HisTime1()->GetMaximum()/
                                tube->HisTime2()->GetMaximum());
        FindT0(tube, coef, "Q");
      }
    }
  }
  delete hfile;
}
