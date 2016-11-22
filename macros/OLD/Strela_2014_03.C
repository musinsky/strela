#ifndef __CINT__
#include <TROOT.h>

void Strela_2014_03()
#endif

{
  if (gStrela) return;
  new TStrela("strela");
  gROOT->Macro("VME_2014_03.C");
  TStrawCham::ShiftAdc(TEventTdc::GetTrigOffset());  // 8000
  TStrawCham::TrigNadc(TEventTdc::GetTrigChannel()); // 58
  gStrela->SetSeance("2014_03");
  gStrela->Init();
  gStrela->SetPath("/mnt/free1/data/2014_03");
  gStrela->SetRun(51);

  TStrawTracker *t1 = gStrela->StrawCham()->FindTracker("tracker_1");
  t1->SetPrecision(1);
  t1->SetMinNHitsAdvanced(4);
  TStrawTracker *t2 = gStrela->StrawCham()->FindTracker("tracker_2");
  t2->SetPrecision(1);
  t2->SetMinNHitsAdvanced(4);
  TStrawTracker *t4 = gStrela->StrawCham()->FindTracker("tracker_4");
  t4->SetPrecision(1);
  t4->SetMinNHitsAdvanced(4);
  TStrawTracker *t11 = gStrela->StrawCham()->FindTracker("tracker_11");
  t11->SetPrecision(1);
  t11->SetMinNHits(4);
  t11->SetTrackMinNHits(4);
  t11->SetMinDistPair(t1->GetMinDistPair());
  TStrawTracker *t12 = gStrela->StrawCham()->FindTracker("tracker_12");
  t12->SetPrecision(1);
  t12->SetMinNHits(4);
  t12->SetTrackMinNHits(4);
  t12->SetMinDistPair(t1->GetMinDistPair());

  t1->SetMaxResTan(0.100);
  t2->SetMaxResTan(0.100);
  t4->SetMaxResTan(0.100);
  t11->SetMaxResTan(0.100);
  t12->SetMaxResTan(0.100);

  gROOT->LoadMacro("macros/DisplayTracker.C+");
  DisplayTracker(t2, t4, t12, t1, 0, t11);
  gStrela->GetDisplay()->GetCanvas()->SetWindowSize(650, 600);
  t1->GetTube(0)->SetShowHistograms();

  gStrela->StrawCham()->SetTubesTimes(0, 2470-100, 2470-100+4752+500);
  TStrawTube::BaseT0(-2470);
  TStrawTube::DriftVel(2.1/4752);

  return;
  TStrawMulti *m10 = new TStrawMulti("m10");
  TFile *fs = new TFile("spline_51-57.root", "READ");
  m10->ReadWriteSpline(fs);
  delete fs;
  m10->AddAnything(t1, t2, t4, t11, t12, 0);
}
