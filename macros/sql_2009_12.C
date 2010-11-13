// root[] .x macros/sql_2009_12.C

// Author: Jan Musinsky
// 24/06/2010

// connectors enumeration in PhTDC and TDC96 modules
//
//  2  3               2
//       => PhTDC      1 => TDC96
//  0  1               0
// ?! order is important ?!

void sql_2009_12(Bool_t print = kTRUE)
{
  gROOT->LoadMacro("macros/sql_vme.cxx+");
  gROOT->Macro("VME_2009_12.C");
  list->Clear();

  // small chambers
  TString name1y[] = {"1y", "1y^", "2y", "2y^"};
  TSmall *cham1y = new TSmall(name1y);
  cham1y->SetTracker(1);
  cham1y->SetZ(0.0);

  TString name1x[] = {"1x", "1x^", "2x", "2x^"};
  TSmall *cham1x = new TSmall(name1x);
  cham1x->SetTracker(2);
  cham1x->SetZ(0.0);

  TString name2[] = {"3y", "3y^", "3x", "3x^"};
  TSmall *cham2 = new TSmall(name2);
  cham2->SetTracker2(3, 4);
  cham2->SetZ2(0.0, 0.0);

  TString namer[] = {"1v", "1v^", "1u", "1u^"}; // rotate
  TSmall *chamr = new TSmall(namer);
  chamr->SetTracker2(0, 0);
  chamr->SetZ2(0.0, 0.0);

  TString name3[] = {"4x", "5x", "4x^", "5x^"};
  TSmall *cham3 = new TSmall(name3);
  cham3->SetTracker(5);
  cham3->SetZ(0.0);

  TString name4[] = {"6y", "6y^", "6x", "6x^"};
  TSmall *cham4 = new TSmall(name4);
  cham4->SetTracker2(3, 4);
  cham4->SetZ2(59.4, 59.4);

  cham1y->SetConnect(0, 0);              // cable 1y (module 0, connector 0)
  cham1x->SetConnect(0, 1);              // cable 2y (module 0, connector 1)
  cham2->SetConnect(2, 0, "odd");        // cable 3y (module 2, connector 0)
  chamr->SetConnect(2, 0, "even");       // cable 1x (module 2, connector 0)
  cham3->SetConnect(2, 1, "odd, onlyx"); // cable 2x (module 2, connector 1)
  cham4->SetConnect(2, 1, "even");       // cable 3x (module 2, connector 1)

  // big chambers
  TString name10y[] = {"10y", "10y^", "11y", "11y^"};
  TBig *cham10y = new TBig(name10y);
  cham10y->SetTracker(10);
  cham10y->SetZ(0.0);

  TString name10x[] = {"10x", "10x^", "11x", "11x^"};
  TBig *cham10x = new TBig(name10x);
  cham10x->SetTracker(11);
  cham10x->SetZ(0.0);

  TString name10r[] = {"10v^", "10v", "10u^", "10u"}; // rotate
  TBig *cham10r = new TBig(name10r);
  cham10r->SetTracker2(0, 0);
  cham10r->SetZ2(0.0, 0.0);
  cham10r->SetDirect2(-1, -1); // backwards

  cham10y->SetConnect1(1, 0, "odd");        // cable 4 (module 1, connector 0)
  cham10y->SetConnect2(1, 0, "even");       // cable 3 (module 1, connector 0)
  cham10x->SetConnect1(1, 1, "odd");        // cable 2 (module 1, connector 1)
  cham10x->SetConnect2(1, 1, "even");       // cable 1 (module 1, connector 1)
  cham10r->SetConnect1(1, 2, "even, half"); // cable 5 (module 1, connector 2)
  cham10r->SetConnect2(1, 2, "odd, half");  // cable 6 (module 1, connector 2)

  if (!print) return;
  TString fn;
  const char *what[2]={"layers", "channels"};
  Printf(gVME->GetName());
  TROOT::DecreaseDirLevel();
  for (Int_t i = 0; i < 2; i++) {
    fn = Form("2009_12_%s.sql", what[i]);
    gROOT->ProcessLine(Form("list->Print(\"%s\"); > %s", what[i], fn.Data()));
    gSystem->Exec(Form("sed -i -e '1d' %s", fn.Data())); // remove first line
    Printf("wrote file '%s'", fn.Data());
  }
}
