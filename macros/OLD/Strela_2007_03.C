// root[] .x Strela_2007_03.C

// Author: Jan Musinsky
// 24/06/2008

{
  if (gStrela) return;
  new TStrela("strela");
  gErrorIgnoreLevel = 1001;
  delete gStrela->WireCham();
  gErrorIgnoreLevel = -1;
  TStrawCham::TrigNadc(362);
  TStrawCham::ShiftAdc(9000);
  gROOT->Macro("VME_2007_03.C");

  gStrela->SetSeance("2007_03");
  gStrela->Init();
  gStrela->SetPath("/free1/data/2007_03");
  gStrela->SetRun(126);

  // small x, before target
  TStrawTracker *t1 = gStrela->StrawCham()->FindTracker("tracker_1");
  t1->SetPrecision(2);
  t1->SetMinNHitsAdvanced(4);
  t1->SetMaxChi2Tan(0.02);
  t1->SetMaxDisZTan(1.0);
  t1->SetMaxResTan(2.0);

  // small y, before target
  TStrawTracker *t2 = gStrela->StrawCham()->FindTracker("tracker_2");
  t2->SetPrecision(2);
  t2->SetMinNHitsAdvanced(4);
  t2->SetMaxChi2Tan(0.02);
  t2->SetMaxDisZTan(1.0);
  t2->SetMaxResTan(2.0);

  // smalls x, between target and magnet
  TStrawTracker *t3 = gStrela->StrawCham()->FindTracker("tracker_3");
  t3->SetPrecision(2);
  t3->SetMinNHitsAdvanced(4);
  t3->SetMaxChi2Tan(0.02);
  t3->SetMaxDisZTan(1.0);
  t3->SetMaxResTan(2.0);

  // smalls y, between target and magnet
  TStrawTracker *t4 = gStrela->StrawCham()->FindTracker("tracker_4");
  t4->SetPrecision(2);
  t4->SetMinNHitsAdvanced(4);
  t4->SetMaxChi2Tan(0.02);
  t4->SetMaxDisZTan(1.0);
  t4->SetMaxResTan(2.0);

  // smalls onlyx, between target and magnet
  TStrawTracker *t5 = gStrela->StrawCham()->FindTracker("tracker_5");
  t5->SetPrecision(2);
  t5->SetMinNHitsAdvanced(4);
  t5->SetMaxChi2Tan(0.02);
  t5->SetMaxDisZTan(1.0);
  t5->SetMaxResTan(2.0);

  // big x, after magnet
  TStrawTracker *t10 = gStrela->StrawCham()->FindTracker("tracker_10");
  t10->SetPrecision(2);
  t10->SetMinNHitsAdvanced(4);
  t10->SetMaxChi2Tan(0.02);
  t10->SetMaxDisZTan(1.0);
  t10->SetMaxResTan(2.0);

  // big y, after magnet
  TStrawTracker *t11 = gStrela->StrawCham()->FindTracker("tracker_11");
  t11->SetPrecision(2);
  t11->SetMinNHitsAdvanced(4);
  t11->SetMaxChi2Tan(0.02);
  t11->SetMaxDisZTan(1.0);
  t11->SetMaxResTan(2.0);

  TStrelaDisplay *display = gStrela->GetDisplay();
  gROOT->LoadMacro("macros/DisplayTracker.C");
  DisplayTracker();
  //  DisplayTracker(2, 1, t10, t11);
  //  DisplayTracker(2, 1, t1, t2);
  //  DisplayTracker(2, 2, t1, t2, t10, t11);
  //  DisplayTracker(t10);

  //  t1->SetMinNHits(9999);
  //  t2->SetMinNHits(9999);
  //  t3->SetMinNHits(9999);
  //  t4->SetMinNHits(9999);
  //  t5->SetMinNHits(9999);
  //  t10->SetMinNHits(9999);
  //  t11->SetMinNHits(9999);

  return;
  TStrawMulti *multi1 = new TStrawMulti("multi_1", "");
  TStrawMulti *multi2 = new TStrawMulti("multi_2", "");
  TStrawMulti *multi3 = new TStrawMulti("multi_3", "");
  TStrawMulti *multi4 = new TStrawMulti("multi_4", "");
  multi1->Add(gStrela->StrawCham()->FindLayer("dch_1x"));
  multi2->Add(gStrela->StrawCham()->FindLayer("dch_1x^"));
  multi3->Add(gStrela->StrawCham()->FindLayer("dch_2x"));
  multi4->Add(gStrela->StrawCham()->FindLayer("dch_2x^"));

  TStrawMulti *multi5 = new TStrawMulti("multi_5", "");
  multi5->Add(t2);

  TStrawMulti *multi10 = new TStrawMulti("multi_10", "");
  TStrawMulti *multi11 = new TStrawMulti("multi_11", "");
  multi10->Add(t10);
  multi11->Add(t11);

  /*
  TStrawMulti *multi3 = new TStrawMulti("multi_3", "between 3");
  multi3->Add(gStrela->StrawCham()->FindLayer("dch_3x"));
  multi3->Add(gStrela->StrawCham()->FindLayer("dch_3x^"));
  multi3->Add(gStrela->StrawCham()->FindLayer("dch_3y"));
  multi3->Add(gStrela->StrawCham()->FindLayer("dch_3y^"));

  TStrawMulti *multi4 = new TStrawMulti("multi_4", "between 4");
  multi4->Add(gStrela->StrawCham()->FindLayer("dch_4x"));
  multi4->Add(gStrela->StrawCham()->FindLayer("dch_5x"));
  multi4->Add(gStrela->StrawCham()->FindLayer("dch_4x^"));
  multi4->Add(gStrela->StrawCham()->FindLayer("dch_5x^"));

  TStrawMulti *multi6 = new TStrawMulti("multi_6", "between 6");
  multi6->Add(gStrela->StrawCham()->FindLayer("dch_6x"));
  multi6->Add(gStrela->StrawCham()->FindLayer("dch_6x^"));
  multi6->Add(gStrela->StrawCham()->FindLayer("dch_6y"));
  multi6->Add(gStrela->StrawCham()->FindLayer("dch_6y^"));

  TStrawMulti *multi10 = new TStrawMulti("multi_10", "after magnet");
  multi10->Add(t10);
  multi10->Add(t11);
  */
}
