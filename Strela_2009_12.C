// Author: Jan Musinsky
// 17/11/2010

/*
  .x Strela_2009_12.C
*/

{
  if (gStrela) return;
  new TStrela("strela");
  gErrorIgnoreLevel = 1001;
  delete gStrela->WireCham();
  gErrorIgnoreLevel = -1;
  //  TStrawCham::TrigNadc(360); // no more need
  gROOT->Macro("VME_2009_12.C");

  gStrela->SetSeance("2009_12");
  gStrela->Init();
  gStrela->SetPath("/free1/data/2009_12");
  gStrela->SetRun(191);

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

  //  m1 = new TStrawMulti("m1");
  //  m1->Add(t1);
  //  m2 = new TStrawMulti("m2");
  //  m2->Add(t2);

  gROOT->LoadMacro("macros/DisplayTracker.C");
  //  DisplayTracker(2, 1, t10, t11);
  DisplayTracker();

  gStrela->StrawCham()->SetTubesTime(0, 800, 7000);
  TStrawTube::SetBaseT0(0);
}
