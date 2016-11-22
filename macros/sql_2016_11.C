// Author: Jan Musinsky
// 22/11/2016

/*
  .x macros/sql_2016_11.C
*/

// connectors enumeration in all 4 TDC64V modules
//   3   2      => HPTDC Id = 2 (tdc_num = 1)
//   1   0      => HPTDC Id = 1 (tdc_num = 0)

void sql_2016_11(Bool_t print = kTRUE)
{
  // ROOT6
  R__LOAD_LIBRARY(macros/sql_vme.cxx+);
  // ROOT5
  //gROOT->LoadMacro("macros/sql_vme.cxx+");
  gROOT->Macro("VME_2016_11.C");
  list6->Clear();

  // small (4y, 4x) chamber
  TString name1y[] = {"1y", "1y^", "2y", "2y^"};
  TSmall *cham1y = new TSmall(name1y);
  cham1y->SetTracker(1);
  cham1y->SetZ(0.0);

  TString name1x[] = {"1x", "1x^", "2x", "2x^"};
  TSmall *cham1x = new TSmall(name1x);
  cham1x->SetTracker(2);
  cham1x->SetZ(0.0);

  cham1y->SetConnect(7, 0);             // module 7, connector 0 (cable 2x)
  cham1x->SetConnect(7, 1);             // module 7, connector 1 (cable 3x)

  // big (4y, 4x) chamber
  TString name10y[] = {"10y", "10y^", "11y", "11y^"};
  TBig *cham10y = new TBig(name10y);
  cham10y->SetTracker(11);
  cham10y->SetZ(0.0);

  TString name10x[] = {"10x", "10x^", "11x", "11x^"};
  TBig *cham10x = new TBig(name10x);
  cham10x->SetTracker(12);
  cham10x->SetZ(0.0);

  // SetConnect1 must be call before SetConnect2
  cham10y->SetConnect1(9, 3);           // module 9, connector 3 (cable 4) "down"
  cham10y->SetConnect2(9, 2);           // module 9, connector 2 (cable 3) "up"
  cham10x->SetConnect1(9, 1);           // module 9, connector 1 (cable 2) "right"
  cham10x->SetConnect2(9, 0);           // module 9, connector 0 (cable 1) "left"

  // big (2y, 2x) chamber (previously rotated)
  TString name12[] = {"12y^", "12y", "12x^", "12x"};
  TBig *cham12 = new TBig(name12);
  cham12->SetTracker2(11, 12);
  cham12->SetZ2(31.4, 31.4);
  cham12->SetDirect2(-1, -1); // reverse

  // SetConnect1 must be call before SetConnect2
  cham12->SetConnect1(11, 0, "half");   // module 11, connector 0 (cable 5)
  cham12->SetConnect2(11, 1, "half");   // module 11, connector 1 (cable 6)

  if (!print) return;
  TString fn0 = gSystem->BaseName(gInterpreter->GetCurrentMacroName());
  fn0.ReplaceAll("sql_", "");
  fn0.ReplaceAll(".C", "");
  TString fn;
  const char *what[2]={"layers", "channels"};
  Printf("%s", gVME->GetName());
  TROOT::DecreaseDirLevel();
  for (Int_t i = 0; i < 2; i++) {
    fn = TString::Format("%s_%s.sql", fn0.Data(), what[i]);
    // ROOT6
    gROOT->ProcessLine(TString::Format(".> %s", fn.Data()));
    gROOT->ProcessLine(TString::Format("list6->Print(\"%s\");", what[i]));
    gROOT->ProcessLine(TString::Format(".>")); // problem
    // ROOT5
    //gROOT->ProcessLine(Form("list6->Print(\"%s\"); > %s", what[i], fn.Data()));
    gSystem->Exec(Form("sed -i -e '1d' %s", fn.Data())); // remove first line
    Printf("wrote file '%s'", fn.Data());
  }
  // ROOT6
  gROOT->ProcessLine(TString::Format(".q")); // ROOT6 has problem with redirect output
}
