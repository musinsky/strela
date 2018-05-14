{
  gROOT->Macro("VME_2016_11.C");
  rd =  new TVMERawData();
  rd->SetTreeFileName("root");
  const char *fn = "/mnt/free1/data_raw/2016_11";
  for (Int_t r = 90; r < 100; r++) {
    //    Printf("%s", TString::Format("%s/run%03d.dat", fn, r).Data());
    rd->DecodeFile(TString::Format("%s/run%03d.dat", fn, r).Data());
  }
}
