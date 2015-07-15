// Author: Jan Musinsky
// 15/07/2015

{
  if (TClass::GetClass("TVME") || TClass::GetClass("TVMEMonitor") ||
      TClass::GetClass("TStrela")) {
    Printf("Libraries was already loaded");
    return;
  }

  gSystem->AddIncludePath("-Iinclude");
  gSystem->SetBuildDir("build", kTRUE);

  gSystem->Load("libGpad.so");
  gSystem->Load("libSQLIO.so");
  gSystem->Load("libTree.so");
  gSystem->Load("lib/libVME.so");
  gSystem->Load("lib/libMonitor.so");
  gSystem->Load("lib/libStrela.so");

  gStyle->SetGridColor(kGray);
  gStyle->SetPalette(55);
}
