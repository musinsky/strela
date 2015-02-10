// Author: Jan Musinsky
// 10/02/2015

{
  if (TClass::GetClass("TVME") || TClass::GetClass("TStrela")) {
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

  gStyle->SetGridColor(kGray);
  gStyle->SetPalette(55);
}
