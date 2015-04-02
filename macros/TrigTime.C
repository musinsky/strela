// Author: Jan Musinsky
// 02/04/2015

/*
  .L macros/TrigTime.C+
  TrigTime("run123.root")
 */

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TStyle.h>

//______________________________________________________________________________
void TrigTime(TTree *tree)
{
  if (!tree) return;

  TH1F *ht = new TH1F("ht", "", 100, 0.0, 0.0);
  tree->Draw("EventTdc.fTrigTime>>ht");
  Int_t xmin = ht->GetBinCenter(ht->FindFirstBinAbove(ht->GetMaximum()*0.1));
  Int_t xmax = ht->GetBinCenter(ht->FindLastBinAbove(ht->GetMaximum()*0.1));
  Int_t width = xmax - xmin; // always around 500

  xmin = xmin - 0.1*width;
  xmin = (xmin + 25)/50*50; // rounding to nearest 50
  xmax = xmax + 0.1*width;
  xmax = (xmax + 25)/50*50; // rounding to nearest 50
  width = xmax - xmin;

  delete ht;
  ht = new TH1F("ht", "", width, xmin, xmax);
  tree->Draw("EventTdc.fTrigTime>>ht");
  gStyle->SetOptStat(111111);
  ht->Draw();

  xmin = ht->GetBinCenter(ht->FindFirstBinAbove(ht->GetMaximum()*0.5)) + 0.5;
  xmax = ht->GetBinCenter(ht->FindLastBinAbove(ht->GetMaximum()*0.5)) + 0.5;
  width = xmax - xmin;
  Int_t mean = xmin + width/2.0 + 0.5;
  Printf("(%d, %d) %d => %d", xmin, xmax, width, mean);
}
//______________________________________________________________________________
void TrigTime(const char *fname)
{
  TFile *file = TFile::Open(fname, "READ");
  if (!file) return;
  TTree *tree = (TTree *)file->Get("pp");
  if (!tree) return;

  TrigTime(tree);
}
