// Author: Jan Musinsky
// 04/03/2014

/*
  .x macros/sql_2014_03.C
 */

// connectors enumeration in all 4 TDC64V modules
//   3   2      => HPTDC Id = 2 (tdc_num = 1)
//   1   0      => HPTDC Id = 1 (tdc_num = 0)

void sql_2014_03(Bool_t print = kTRUE)
{
  gROOT->LoadMacro("macros/sql_vme.cxx+");
  gROOT->Macro("VME_2014_03.C");
  list->Clear();

  // small (4y, 4x) chambers
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

  // big (4y, 4x) chambers
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

  // big (2y, 2x) chambers (previously rotate)
  TString name12[] = {"12y^", "12y", "12x^", "12x"};
  TBig *cham12 = new TBig(name12);
  cham12->SetTracker2(12, 13);
  cham12->SetZ2(0.0, 0.0);
  //  cham12->SetDirect2(-1, -1); // backwards

  // SetConnect1 must be call before SetConnect2
  cham12->SetConnect1(11, 0, "half");   // cable 5 (module 11, connector 0)
  // ?! VICE VERSA ?! checking !!!
  cham12->SetConnect2(11, 1, "half");   // cable 6 (module 11, connector 1)

  // small (only x) chambers
  TString name3[] = {"4x", "5x", "4x^", "5x^"};
  TSmall *cham3 = new TSmall(name3);
  cham3->SetTracker(5);
  cham3->SetZ(0.0);

  cham3->SetConnect(13, 2, "onlyx");    // cable 3y (module 13, connector 2)

  if (!print) return;
  TString fn;
  const char *what[2]={"layers", "channels"};
  Printf(gVME->GetName());
  TROOT::DecreaseDirLevel();
  for (Int_t i = 0; i < 2; i++) {
    fn = Form("2014_03_%s.sql", what[i]); // !!!
    gROOT->ProcessLine(Form("list->Print(\"%s\"); > %s", what[i], fn.Data()));
    gSystem->Exec(Form("sed -i -e '1d' %s", fn.Data())); // remove first line
    Printf("wrote file '%s'", fn.Data());
  }
}
