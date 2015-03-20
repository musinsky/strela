#ifndef __CINT__
#include <TROOT.h>

void Strela_BIG()
#endif

{
  if (gStrela) return;
  new TStrela("strela_BIG");
  gROOT->Macro("VME_2014_03.C");
  TStrawCham::ShiftAdc(TEventTdc::GetTrigOffset());  // 8000
  TStrawCham::TrigNadc(TEventTdc::GetTrigChannel()); // 58
  gStrela->SetSeance("2014_03_BIG");
  gStrela->Init();
  gStrela->SetPath("BIG");
  //  gStrela->SetRun(51);
  TChain chain("pp");
  // CH2
  chain.Add("BIG/run051vme_2014-03-03_10-40-29.root");
  chain.Add("BIG/run052vme_2014-03-03_10-50-19.root");
  chain.Add("BIG/run053vme_2014-03-03_10-58-20.root");
  chain.Add("BIG/run054vme_2014-03-03_11-06-37.root");
  chain.Add("BIG/run055vme_2014-03-03_11-14-17.root");
  chain.Add("BIG/run056vme_2014-03-03_11-22-55.root");
  //  // Empty
  //  chain.Add("BIG/run064vme_2014-03-03_12-44-28.root");
  //  chain.Add("BIG/run065vme_2014-03-03_12-57-12.root");
  //  chain.Add("BIG/run066vme_2014-03-03_13-06-30.root");
  //  chain.Add("BIG/run067vme_2014-03-03_13-14-14.root");
  //  chain.Add("BIG/run068vme_2014-03-03_13-23-02.root");
  gStrela->ChangeBranchAddress(&chain);

  TStrawTracker *t11 = gStrela->StrawCham()->FindTracker("tracker_11"); // Y
  t11->SetPrecision(2);
  t11->SetMinNHitsAdvanced(4);
  TStrawTracker *t12 = gStrela->StrawCham()->FindTracker("tracker_12"); // X
  t12->SetPrecision(2);
  t12->SetMinNHitsAdvanced(4);

  t11->SetMaxResTan(0.075);
  t12->SetMaxResTan(0.075);
  t11->SetMaxChi2Tan(0.005);
  t12->SetMaxChi2Tan(0.005);

  gROOT->LoadMacro("macros/DisplayTracker.C+");
  DisplayTracker(1, 2, t12, t11);
  gStrela->GetDisplay()->GetCanvas()->SetWindowSize(150, 600);
  t11->GetTube(0)->SetShowHistograms();

  gStrela->StrawCham()->SetTubesTimes(0, 2470-200, 2470-200+4752+1500);
  TStrawTube::BaseT0(-2470);
  TStrawTube::DriftVel(2.1/4752);

  return;
  TStrawMulti *m10 = new TStrawMulti("m10");
  TFile *fs = new TFile("spline_51-57.root", "READ");
  m10->ReadWriteSpline(fs);
  delete fs;
  m10->AddAnything(t11, t12, 0);

  return;
  TStrawMulti *m11  = new TStrawMulti("m11");
  TStrawMulti *m12  = new TStrawMulti("m12");
  m11->Add(t11);
  m12->Add(t12);
  TFile *fs11 = new TFile("spline_m11.root", "READ");
  m11->ReadWriteSpline(fs11);
  delete fs11;
  TFile *fs12 = new TFile("spline_m12.root", "READ");
  m12->ReadWriteSpline(fs12);
  delete fs12;
}
