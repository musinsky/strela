// root[] .x Strela_2013_12.C

// Author: Jan Musinsky
// 04/12/2013

{
  if (gStrela) return;
  new TStrela("strela");
    gROOT->Macro("VME_2013_12.C");
  gErrorIgnoreLevel = -1;
  TStrawCham::ShiftAdc(7000);
  TStrawCham::TrigNadc(58);

  gStrela->SetSeance("2013_12");
  gStrela->Init();
  gStrela->SetPath("/home/musinsky/strela");
  gStrela->SetRun(100);

  TStrawTracker *t1 = gStrela->StrawCham()->FindTracker("tracker_1");
  t1->SetPrecision(1);
  t1->SetMinNHitsAdvanced(4);
  TStrawTracker *t2 = gStrela->StrawCham()->FindTracker("tracker_2");
  t2->SetPrecision(1);
  t2->SetMinNHitsAdvanced(4);
  TStrawTracker *t10 = gStrela->StrawCham()->FindTracker("tracker_10");
  t10->SetPrecision(1);
  t10->SetMinNHitsAdvanced(4);
  TStrawTracker *t11 = gStrela->StrawCham()->FindTracker("tracker_11");
  t11->SetPrecision(1);
  t11->SetMinNHitsAdvanced(4);

  gROOT->LoadMacro("macros/DisplayTracker.C");
  DisplayTracker(2, 2, t1, t10, t2, t11);
  gStrela->GetDisplay()->FindPad(t1)->SetPad( 0.0, 0.6, 0.4, 0.9);
  gStrela->GetDisplay()->FindPad(t10)->SetPad(0.6, 0.5, 1.0, 1.0);
  gStrela->GetDisplay()->FindPad(t2)->SetPad( 0.0, 0.1, 0.4, 0.4);
  gStrela->GetDisplay()->FindPad(t11)->SetPad(0.6, 0.0, 1.0, 0.5);
  gStrela->GetDisplay()->GetCanvas()->SetWindowSize(450, 600);
  t1->GetTube(0)->SetShowHistograms();

  gStrela->StrawCham()->SetTubesTimes(0, 1750, 1750+4800);
  TStrawTube::BaseT0(-1750);
  TStrawTube::DriftVel(2.1/4600);

  m1  = new TStrawMulti("m1");
  m2  = new TStrawMulti("m2");
  m10 = new TStrawMulti("m10");
  m11 = new TStrawMulti("m11");
  m1->Add(t1);
  m2->Add(t2);
  m10->Add(t10);
  m11->Add(t11);


  break;

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
