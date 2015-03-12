// Author: Jan Musinsky
// 09/07/2014

/*
  .x Strela.C
  .L macros/MonteCarlo.C+
  GenerateRun()
 */

#include <TFile.h>
#include <TRandom.h>

#include "TTDCEvent.h"
#include "TStrawTracker.h"

void GenerateRun(Int_t run = 0, Int_t ne = 100000, Bool_t analyze = kTRUE)
{
  if (!gStrela || !gVME) return;

  // not correctly solution (see in TStrela::SetRun)
  if (gStrela->GetChain() && gStrela->GetChain()->GetCurrentFile())
    delete gStrela->GetChain()->GetCurrentFile();
  TFile *file = new TFile(TString::Format("run%03d.root", run), "RECREATE");
  TTree *tree = new TTree("pp", "Monte Carlo");
  TTDCEvent *event = new TTDCEvent();
  tree->Branch("TDCEvent", &event);
  // need assign gStrela->GetChain() and gStrela->GemEvent()
  gStrela->ChangeBranchAddress(tree);

  TStrawTracker *t1  = gStrela->StrawCham()->FindTracker("tracker_1");
  TStrawTracker *t2  = gStrela->StrawCham()->FindTracker("tracker_2");
  TStrawTracker *t11 = gStrela->StrawCham()->FindTracker("tracker_11");
  TStrawTracker *t12 = gStrela->StrawCham()->FindTracker("tracker_12");

  Int_t tshift = 2470, tmax = 4752;
  TStrawTube::BaseT0(-tshift);
  TStrawTube::DriftVel(2.1/tmax);
  gStrela->StrawCham()->SetTubesTimes(0, tshift, tshift + tmax);

  // set particular T0 time (and try modifying sigma of "b" parameter)
  Bool_t partT0 = kFALSE;
  if (partT0) {
    t11->GetTube(2)->SetT0(20); t11->GetTube(3)->SetT0(30);
    t11->GetTube(4)->SetT0(45); t11->GetTube(5)->SetT0(55);
    t12->GetTube(2)->SetT0(10); t12->GetTube(3)->SetT0(20);
    t12->GetTube(4)->SetT0(30); t12->GetTube(5)->SetT0(40);
  }

  Double_t a, b;
  for (Int_t i = 0; i < ne; i++) {
    gStrela->TDCEvent()->Clear();

    // generate trigger hit
    if (!(TTDCEvent::GetTrigChannel() < 0))
      gStrela->TDCEvent()->AddTDCHit(TTDCEvent::GetTrigChannel(), TTDCEvent::GetTrigOffset());

    // bad MC events
    if (t1) t1->GenerateHits(gRandom->Gaus( 0.000, 0.03), gRandom->Uniform(-7.0, 7.0), 0.01);
    if (t2) t2->GenerateHits(gRandom->Gaus(-0.025, 0.01), gRandom->Uniform(-5.0, 5.0), 0.03);

    // good MC events
    a = gRandom->Gaus(0.015, 0.015);
    b = gRandom->Gaus(-8.0, 0.75) - a*-333.0   ; // sigma (from generated gauss) is important for T0 analyze
    if (t11) t11->GenerateHits(a, b, 0.010);
    a = gRandom->Gaus(-0.020, 0.030);
    b = gRandom->Gaus(10.0, 2.00) - a*-343.0;
    if (t12) t12->GenerateHits(a, b, 0.012);
    // "beam tail" (only for X chamber)
    a = gRandom->Gaus(0.12, 0.030);
    b = gRandom->Gaus(-18.0, 1.50) - a*-343.0;
    if (t12) t12->GenerateHits(a, b, 0.012);

    tree->Fill();
    //    gStrela->AnalyzeEntry(i);
  }
  file->Write();
  delete file;
  if (!analyze) return;

  // see in TStrela::SetRun, but dont use gStrela->SetRun()
  // gStrela->GetChain() is deleted from memory, but still refers to bad (old) address
  file = TFile::Open(TString::Format("run%03d.root", run), "READ");
  tree = (TChain *)file->Get("pp");
  gStrela->ChangeBranchAddress(tree);

  // reset min, max time
  gStrela->StrawCham()->SetTubesTimes(0, 0, 9999);
  // reset particular T0 time
  if (partT0) gStrela->StrawCham()->SetTubesTimes(3, 0);
  gStrela->AnalyzeEntries();
}
