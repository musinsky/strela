// Author: Jan Musinsky
// 13/06/2014

/*
  .x Strela.C
  .L macros/MonteCarlo.C+
  GenerateRun()
 */

#include <TFile.h>
#include <TRandom.h>

#include "TVMEEvent.h"
#include "TStrawTracker.h"

void GenerateRun(Int_t run = 0, Int_t ne = 100000, Bool_t analyze = kTRUE)
{
  if (!gStrela || !gVME) return;

  // not correctly solution (see in TStrela::SetRun)
  if (gStrela->GetChain() && gStrela->GetChain()->GetCurrentFile())
    delete gStrela->GetChain()->GetCurrentFile();
  TFile *file = new TFile(TString::Format("run%03d.root", run), "RECREATE");
  TTree *tree = new TTree("pp", "Monte Carlo");
  TVMEEvent *event = new TVMEEvent();
  tree->Branch("VMEEvent", &event);
  // need assign gStrela->GetChain() and gStrela->GemEvent()
  gStrela->ChangeBranchAddress(tree);

  TStrawTracker *t1  = gStrela->StrawCham()->FindTracker("tracker_1");
  TStrawTracker *t2  = gStrela->StrawCham()->FindTracker("tracker_2");
  TStrawTracker *t11 = gStrela->StrawCham()->FindTracker("tracker_11");
  TStrawTracker *t12 = gStrela->StrawCham()->FindTracker("tracker_12");

  Int_t tshift = 2500, tmax = 4750;
  TStrawTube::BaseT0(-tshift);
  gStrela->StrawCham()->SetTubesTimes(0, tshift, tshift + tmax);
  TStrawTube::DriftVel(2.1/tmax);

  for (Int_t i = 0; i < ne; i++) {
    gStrela->VMEEvent()->Clear();
    if (t1)   t1->GenerateHits(gRandom->Gaus(0, 0.033),      gRandom->Uniform(-7, 7), 0.01);
    if (t2)   t2->GenerateHits(gRandom->Gaus(-0.025, 0.01),  gRandom->Uniform(-5, 5), 0.03);
    if (t11) t11->GenerateHits(gRandom->Gaus(0.025, 0.025),  gRandom->Gaus(0, 10),    0.01);
    if (t12) t12->GenerateHits(gRandom->Gaus(-0.050, 0.010), gRandom->Gaus(-7, 10),   0.01);
    tree->Fill();
    //    gStrela->AnalyzeEntry(i);
  }
  file->Write();
  delete file;

  // see in TStrela::SetRun, but dont use gStrela->SetRun()
  // gStrela->GetChain() is deleted from memory, but still refers to bad (old) address
  file = TFile::Open(TString::Format("run%03d.root", run), "READ");
  tree = (TChain *)file->Get("pp");
  gStrela->ChangeBranchAddress(tree);
  TStrawCham::TrigNadc(-1);
  if (analyze) gStrela->AnalyzeEntries();
}
