// root[] .x macros/sql_2007_03.C

// Author: Jan Musinsky
// 24/06/2008

void sql_2007_03(Bool_t print = kTRUE)
{
  gROOT->LoadMacro("macros/sql_vme.cxx+");
  gROOT->Macro("VME_2007_03.C");
  list->Clear();

  // small chambers
  TString name1y[] = {"1y", "1y^", "2y", "2y^"};
  TSmall *cham1y = new TSmall(name1y);
  cham1y->SetTracker(2);
  cham1y->SetZ(0.0);

  TString name1x[] = {"1x", "1x^", "2x", "2x^"};
  TSmall *cham1x = new TSmall(name1x);
  cham1x->SetTracker(1);
  cham1x->SetZ(0.0);

  TString name2[] = {"3y", "3y^", "3x", "3x^"};
  TSmall *cham2 = new TSmall(name2);
  cham2->SetTracker2(4, 3);
  cham2->SetZ2(0.0, 0.0);

  TString name3[] = {"4x", "5x", "4x^", "5x^"};
  TSmall *cham3 = new TSmall(name3);
  cham3->SetTracker(5);
  cham3->SetZ(0.0);

  TString name4[] = {"6y", "6y^", "6x", "6x^"};
  TSmall *cham4 = new TSmall(name4);
  cham4->SetTracker2(4, 3);
  cham4->SetZ2(59.4, 59.4);

  cham1y->SetConnect(0, 0);         // cable 1y
  cham1x->SetConnect(0, 1);         // cable 1x
  cham2->SetConnect(0, 3);          // cable 2y
  cham3->SetConnect(1, 2, "onlyx"); // cable 3y
  cham4->SetConnect(1, 3);          // cable 3x

  // big chambers
  TString name10y[] = {"10y", "10y^", "11y", "11y^"};
  TBig *cham10y = new TBig(name10y);
  cham10y->SetTracker(11);
  cham10y->SetZ(0.0);

  TString name10x[] = {"10x", "10x^", "11x", "11x^"};
  TBig *cham10x = new TBig(name10x);
  cham10x->SetTracker(10);
  cham10x->SetZ(0.0);

  cham10y->SetConnect1(2, 2); // cable 5
  cham10y->SetConnect2(2, 3); // cable red
  cham10x->SetConnect1(2, 0); // cable by1
  cham10x->SetConnect2(2, 1); // cable bx1

  if (!print) return;
  gROOT->ProcessLine("list->Print(\"\", \"layers\"); > 2007_03_layers.sql");
  gROOT->ProcessLine("list->Print(\"\", \"channels\"); > 2007_03_channels.sql");
}
