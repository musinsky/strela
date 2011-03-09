// root[] .x Strela_2011_03.C

// Author: Jan Musinsky
// 09/03/2011

{
  if (gStrela) return;
  new TStrela("strela");
  gErrorIgnoreLevel = 1001;
  delete gStrela->WireCham();
  gErrorIgnoreLevel = -1;
  //  TStrawCham::TrigNadc(360); // no more need
  gROOT->Macro("VME_2011_03.C");

  gStrela->SetSeance("2011_03");
  gStrela->Init();
  gStrela->SetPath("/free1/data/2011_03");
  gStrela->SetRun(101);

  TStrawTracker *t1 = gStrela->StrawCham()->FindTracker("tracker_1");
  t1->SetPrecision(1);
  t1->SetMinNHitsAdvanced(4);

  TStrawTracker *t2 = gStrela->StrawCham()->FindTracker("tracker_2");
  t2->SetPrecision(1);
  t2->SetMinNHitsAdvanced(4);

  TStrawTracker *t10 = gStrela->StrawCham()->FindTracker("tracker_10");
  t10->SetPrecision(1);
  t10->SetMinNHitsAdvanced(4);
  //  t10->SetMaxChi2Tan(0.02);
  //  t10->SetMaxDisZTan(1.0);
  //  t10->SetMaxResTan(2.0);

  TStrawTracker *t11 = gStrela->StrawCham()->FindTracker("tracker_11");
  t11->SetPrecision(1);
  t11->SetMinNHitsAdvanced(4);
  //  t11->SetMaxChi2Tan(0.02);
  //  t11->SetMaxDisZTan(1.0);
  //  t11->SetMaxResTan(2.0);

  //  gStrela->StrawCham()->SetTubesTimes(0, 570, 570+4700);
  //  TStrawTube::BaseT0(-570);
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

  gROOT->LoadMacro("macros/DisplayTracker.C");
  DisplayTracker(2, 2, t1, t10, t2, t11);
  gStrela->GetDisplay()->FindPad(t1)->SetPad( 0.0, 0.6, 0.4, 0.9);
  gStrela->GetDisplay()->FindPad(t10)->SetPad(0.6, 0.5, 1.0, 1.0);
  gStrela->GetDisplay()->FindPad(t2)->SetPad( 0.0, 0.1, 0.4, 0.4);
  gStrela->GetDisplay()->FindPad(t11)->SetPad(0.6, 0.0, 1.0, 0.5);
}
