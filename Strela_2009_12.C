// root[] .x Strela_2009_12.C

// Author: Jan Musinsky
// 23/06/2010

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

  // 970 + 4300 // t1,t2
  // 880 + 4750 // t5 // 850, 850+4650
  // 840 + 4900 // t10,t11 // koniec s magnetom 840, 840+4800
  // t3,t4 blizsie k t1,t2
  TStrawTracker *t1 = gStrela->StrawCham()->FindTracker("tracker_1");
  t1->SetPrecision(2);
  t1->SetMinNHitsAdvanced(4);

  TStrawTracker *t2 = gStrela->StrawCham()->FindTracker("tracker_2");
  t2->SetPrecision(2);
  t2->SetMinNHitsAdvanced(4);

  TStrawTracker *t3 = gStrela->StrawCham()->FindTracker("tracker_3");
  t3->SetPrecision(2);
  t3->SetMinNHitsAdvanced(4);

  TStrawTracker *t4 = gStrela->StrawCham()->FindTracker("tracker_4");
  t4->SetPrecision(2);
  t4->SetMinNHitsAdvanced(4);

  TStrawTracker *t5 = gStrela->StrawCham()->FindTracker("tracker_5");
  t5->SetPrecision(2);
  t5->SetMinNHitsAdvanced(4);

  TStrawTracker *t10 = gStrela->StrawCham()->FindTracker("tracker_10");
  t10->SetPrecision(2);
  t10->SetMinNHitsAdvanced(4);
  //  t10->SetMaxChi2Tan(0.02);
  //  t10->SetMaxDisZTan(1.0);
  //  t10->SetMaxResTan(2.0);

  TStrawTracker *t11 = gStrela->StrawCham()->FindTracker("tracker_11");
  t11->SetPrecision(2);
  t11->SetMinNHitsAdvanced(4);
  //  t11->SetMaxChi2Tan(0.02);
  //  t11->SetMaxDisZTan(1.0);
  //  t11->SetMaxResTan(2.0);

  //  m1 = new TStrawMulti("m1");
  //  m1->Add(t1); m1->Add(t2); m1->Add(t3); m1->Add(t4); m1->Add(t5);
  //  m1->Add(t3);m1->Add(t4);
  //  f = TFile::Open("spline_OK.root");
  //  m1->ReadWriteSpline(f);
  //  f->Close();

  //  m2 = new TStrawMulti("m2");
  //  m2->Add(t10); m2->Add(t11);
  //  f = TFile::Open("spline3.root");
  //  m2->ReadWriteSpline(f);
  //  f->Close();

  TStrelaDisplay *display = gStrela->GetDisplay();
  gROOT->LoadMacro("macros/DisplayTracker.C");
  //  DisplayTracker(2, 1, t10, t11);
  //  DisplayTracker(2, 1, t1, t2);
  //  DisplayTracker(1, 2, t3, t4);
  DisplayTracker();
}
