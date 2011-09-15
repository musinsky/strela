// Author: Jan Musinsky
// 15/09/2011

{
  if (gStrela) return;
  new TStrela("strela");
  gROOT->Macro("VME_2009_12.C");
  gStrela->SetSeance("2009_12");
  gStrela->Init();
  gStrela->SetPath("/free1/data/2009_12");
  gStrela->SetRun(192);

  TStrawTracker *t1 = gStrela->StrawCham()->FindTracker("tracker_1");
  t1->SetPrecision(1);
  t1->SetMinNHitsAdvanced(4);
  TStrawTracker *t2 = gStrela->StrawCham()->FindTracker("tracker_2");
  t2->SetPrecision(1);
  t2->SetMinNHitsAdvanced(4);
  TStrawTracker *t3 = gStrela->StrawCham()->FindTracker("tracker_3");
  t3->SetPrecision(1);
  t3->SetMinNHitsAdvanced(4);
  TStrawTracker *t4 = gStrela->StrawCham()->FindTracker("tracker_4");
  t4->SetPrecision(1);
  t4->SetMinNHitsAdvanced(4);
  TStrawTracker *t5 = gStrela->StrawCham()->FindTracker("tracker_5");
  t5->SetPrecision(1);
  t5->SetMinNHitsAdvanced(4);
  TStrawTracker *t10 = gStrela->StrawCham()->FindTracker("tracker_10");
  t10->SetPrecision(1);
  t10->SetMinNHitsAdvanced(4);
  TStrawTracker *t11 = gStrela->StrawCham()->FindTracker("tracker_11");
  t11->SetPrecision(1);
  t11->SetMinNHitsAdvanced(4);

  gROOT->LoadMacro("macros/DisplayTracker.C");
  DisplayTracker();
  t1->GetTube(0)->SetShowHistograms();
  gStrela->StrawCham()->SetTubesTimes(3, 880, 0);
  gStrela->StrawCham()->SetTubesTimes(0, 880, 880+4490+250);
//  gStrela->StrawCham()->SetTubesTimes(2, 0, 4490);
  TStrawTube::DriftVel(2.075/4490);


  break;
  gStrela->StrawCham()->SetTubesTimes(0, 800, 7000);
  gStrela->StrawCham()->SetTubesTimes(2, 0, 4700);
  TStrawTube::DriftVel(2.1/4555);
  m1  = new TStrawMulti("m1");
  m2  = new TStrawMulti("m2");
  m10 = new TStrawMulti("m10");
  m11 = new TStrawMulti("m11");
  m1->Add(t1);
  m2->Add(t2);
  m10->Add(t10);
  m11->Add(t11);
}
