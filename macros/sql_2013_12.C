// Author: Jan Musinsky
// 06/12/2013

/*
  .x macros/sql_2013_12.C
 */

// connectors enumeration in all 4 TDC64V modules
//   3   2      => HPTDC Id = 2 (tdc_num = 1)
//   1   0      => HPTDC Id = 1 (tdc_num = 0)

void sql_2013_12(Bool_t print = kTRUE)
{
  gROOT->LoadMacro("macros/sql_vme.cxx+");
  gROOT->Macro("VME_2013_12.C");
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

  cham1y->SetConnect(7, 0);     // cable 2x (module 7, connector 0)
  cham1x->SetConnect(7, 1);     // cable 3x (module 7, connector 1)

  // big chambers
  TString name10y[] = {"10y", "10y^", "11y", "11y^"};
  TBig *cham10y = new TBig(name10y);
  cham10y->SetTracker(10);
  cham10y->SetZ(0.0);

  TString name10x[] = {"10x", "10x^", "11x", "11x^"};
  TBig *cham10x = new TBig(name10x);
  cham10x->SetTracker(11);
  cham10x->SetZ(0.0);

  // SetConnect1 must be call before SetConnect2
  cham10y->SetConnect1(9, 3);   // cable 4 (module 9, connector 3) "down"
  cham10y->SetConnect2(9, 2);   // cable 3 (module 9, connector 2) "up"
  cham10x->SetConnect1(9, 1);   // cable 2 (module 9, connector 1) "right"
  cham10x->SetConnect2(9, 0);   // cable 1 (module 9, connector 0) "left"

  // big rotate chambers
  // module 11, connector 0
  // module 11, connector 1

  // small others chambers
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

  cham2->SetConnect(13, 0);          // cable 3y (module 13, connector 0)
  chamr->SetConnect(13, 1);          // cable 1x (module 13, connector 1)
  cham3->SetConnect(13, 2, "onlyx"); // cable 1y (module 13, connector 2)
  cham4->SetConnect(13, 3);          // cable 2y (module 13, connector 3)

  if (!print) return;
  TString fn;
  const char *what[2]={"layers", "channels"};
  Printf(gVME->GetName());
  TROOT::DecreaseDirLevel();
  for (Int_t i = 0; i < 2; i++) {
    fn = Form("2013_12_%s.sql", what[i]); // !!!
    gROOT->ProcessLine(Form("list->Print(\"%s\"); > %s", what[i], fn.Data()));
    gSystem->Exec(Form("sed -i -e '1d' %s", fn.Data())); // remove first line
    Printf("wrote file '%s'", fn.Data());
  }
}
