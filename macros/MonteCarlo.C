// Author: Jan Musinsky
// 15/10/2011

/*
  .x Strela.C
  .L macros/MonteCarlo.C+
  GenerateRun()
 */

#include <TFile.h>
#include <TRandom.h>

#include "TGemEvent.h"
#include "TStrawTracker.h"

void GenerateRun(Int_t run = 0, Int_t ne = 100000, Bool_t analyze = kTRUE)
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
  TStrawTracker *t3  = gStrela->StrawCham()->FindTracker("tracker_3");
  TStrawTracker *t4  = gStrela->StrawCham()->FindTracker("tracker_4");
  TStrawTracker *t5  = gStrela->StrawCham()->FindTracker("tracker_5");
  TStrawTracker *t10 = gStrela->StrawCham()->FindTracker("tracker_10");
  TStrawTracker *t11 = gStrela->StrawCham()->FindTracker("tracker_11");

  Int_t tmax = 4400;
  gStrela->StrawCham()->SetTubesTimes(3, 0, 0);
  gStrela->StrawCham()->SetTubesTimes(0, 0, 0 + tmax);
  TStrawTube::DriftVel(2.1/tmax);

  for (Int_t i = 0; i < ne; i++) {
    gStrela->GemEvent()->Clear();
    if (t1)   t1->GenerateHits(gRandom->Gaus(0, 0.033),
                               gRandom->Uniform(-7, 7),
                               0.01);
    if (t2)   t2->GenerateHits(gRandom->Gaus(-0.025, 0.01),
                               gRandom->Uniform(-5, 5),
                               0.03);
    if (t3)   t3->GenerateHits(gRandom->Gaus(0, 0.02),
                               gRandom->Uniform(-7, 7),
                               0.015);
    if (t4)   t4->GenerateHits(gRandom->Gaus(0.02, 0.01),
                               gRandom->Gaus(-2, 5),
                               0.01);
    if (t5)   t5->GenerateHits(gRandom->Gaus(0, 0.025),
                               gRandom->Uniform(-7, 7),
                               0.01);
    if (t10) t10->GenerateHits(gRandom->Gaus(0.05, 0.005),
                               gRandom->Uniform(-10, 10),
                               0.01);
    if (t11) t11->GenerateHits(gRandom->Gaus(0, 0.025),
                               gRandom->Gaus(-7, 10),
                               0.01);
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
  if (analyze) gStrela->AnalyzeEntries();
}
