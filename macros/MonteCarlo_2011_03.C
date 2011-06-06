// Author: Jan Musinsky
// 06/06/2011

/*
  .x Strela.C
  .L macros/MonteCarlo_2011_03.C+
  GenerateRun()
 */

#include <TROOT.h>
#include <TFile.h>
#include <TRandom.h>

#include "TGemEvent.h"
#include "TStrawTracker.h"

void GenerateRun(Int_t run = 0, Int_t ne = 100000)
{
  if (!gStrela) return;

  // see in TStrela::SetRun
  // not correctly solution ...
  if (gStrela->GetChain() && gStrela->GetChain()->GetCurrentFile())
    delete gStrela->GetChain()->GetCurrentFile();
  TFile *file = new TFile(Form("run%03d.root", run), "RECREATE");
  TTree *tree = new TTree("pp", "Monte Carlo");
  TGemEvent *geme = new TGemEvent();
  tree->Branch("event", geme->ClassName(), &geme);
  // need assig gStrela->GetChain() and gStrela->GemEvent()
  gStrela->ChangeBranchAddress(tree);

  TStrawTracker *t1  = gStrela->StrawCham()->FindTracker("tracker_1");
  TStrawTracker *t2  = gStrela->StrawCham()->FindTracker("tracker_2");
  TStrawTracker *t10 = gStrela->StrawCham()->FindTracker("tracker_10");
  TStrawTracker *t11 = gStrela->StrawCham()->FindTracker("tracker_11");

  gStrela->StrawCham()->SetTubesTimes(3, 0, 0);
  gStrela->StrawCham()->SetTubesTimes(0, 0, 0 + 4400);
  TStrawTube::DriftVel(2.1/4400);

  Double_t a, b;
  for (Int_t i = 0; i < ne; i++) {
    gStrela->GemEvent()->Clear();
    a = gRandom->Gaus(0, 0.033);
    b = gRandom->Uniform(-7, 7);
    t1->GenerateHits(a, b, 0.01);
    a = gRandom->Gaus(-0.025, 0.01);
    b = gRandom->Uniform(-5, 5);
    t2->GenerateHits(a, b, 0.03);
    a = gRandom->Gaus(0.05, 0.005);
    b = gRandom->Uniform(-10, 10);
    t10->GenerateHits(a, b, 0.015);
    a = gRandom->Gaus(0, 0.025);
    b = gRandom->Uniform(-13, 13);
    t11->GenerateHits(a, b, 0.01);
    tree->Fill();
    //    gStrela->AnalyzeEntry(i);
  }
  tree->Write();
  delete file;

  // see in TStrela::SetRun, but dont use gStrela->SetRun()
  // gStrela->GetChain() is deleted from memory, but still refers to bad (old) address
  file = TFile::Open(Form("run%03d.root", run), "READ");
  tree = (TChain *)file->Get("pp");
  gStrela->ChangeBranchAddress(tree);
}
