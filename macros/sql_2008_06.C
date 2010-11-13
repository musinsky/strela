// root[] .x macros/sql_2008_06.C

// Author: Jan Musinsky
// 24/06/2008

void sql_2008_06(Bool_t print = kTRUE)
{
  gROOT->LoadMacro("macros/sql_vme.cxx+");
  gROOT->Macro("VME_2008_06.C");
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

  cham1y->SetConnect(0, 1);               // cable 1y
  cham1x->SetConnect(0, 0);               // cable 2y
  cham2->SetConnect(2, 2, "even");        // cable 1x
  cham3->SetConnect(2, 1, "onlyx, even"); // cable 3x
  cham4->SetConnect(2, 1, "odd");         // cable 2x

  // big chambers
  TString name10y[] = {"10y", "10y^", "11y", "11y^"};
  TBig *cham10y = new TBig(name10y);
  cham10y->SetTracker(11);
  cham10y->SetZ(0.0);

  TString name10x[] = {"10x", "10x^", "11x", "11x^"};
  TBig *cham10x = new TBig(name10x);
  cham10x->SetTracker(10);
  cham10x->SetZ(0.0);

  cham10y->SetConnect1(1, 0, "odd");  // cable 3a
  cham10y->SetConnect2(1, 0, "even"); // cable 3b
  cham10x->SetConnect1(1, 1, "odd");  // cable 2a
  cham10x->SetConnect2(1, 1, "even"); // cable 2b

  // small rotate chamber
  TString name1r[] = {"1v", "1v^", "1u", "1u^"};
  TSmall *cham1r = new TSmall(name1r);
  cham1r->SetTracker2(0, 0);
  cham1r->SetZ2(0.0, 0.0);
  cham1r->SetConnect(2, 2, "odd"); //cable 3y

  // big rotate chamber
  TString name10r[] = {"10v^", "10v", "10u^", "10u"};
  TBig *cham10r = new TBig(name10r);
  cham10r->SetTracker2(0, 0);
  cham10r->SetZ2(0.0, 0.0);
  cham10r->SetDirect2(-1, -1); // backwards
  cham10r->SetConnect1(1, 2, "half, even"); // cable 1b
  cham10r->SetConnect2(1, 2, "half, odd");  // cable 1a

  if (!print) return;
  gROOT->ProcessLine("list->Print(\"\", \"layers\"); > 2008_06_layers.sql");
  gROOT->ProcessLine("list->Print(\"\", \"channels\"); > 2008_06_channels.sql");
}
