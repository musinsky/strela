// Author: Jan Musinsky
// 27/05/2014

{
  if (gROOT->GetClass("TStrela") || gROOT->GetClass("TVME")) {
    Printf("Libraries was already loaded");
    return;
  }

  gSystem->AddIncludePath("-Iinclude");
  gSystem->SetBuildDir("build", kTRUE);

  gSystem->Load("libGpad.so");
  gSystem->Load("libSQLIO.so");
  gSystem->Load("libTree.so");
  gSystem->Load("lib/libVME.so");
  gSystem->Load("lib/libStrela.so");

  gStyle->SetHistLineColor(kBlack);
  gStyle->SetGridColor(kGray);
}
