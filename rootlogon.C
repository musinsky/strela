// Author: Jan Musinsky
// 22/11/2016

{
  if (TClassTable::GetDict("TVME") || TClassTable::GetDict("TVMEMonitor") ||
      TClassTable::GetDict("TStrela")) {
    Printf("Libraries was already loaded");
    return;
  }

  if (gROOT->GetVersionInt() < 60000) { // ROOT5
    gSystem->AddIncludePath("-Iinclude");
  }
  else {                                // ROOT6
    gInterpreter->AddIncludePath("-Iinclude");
    // or to same with
    // R__ADD_INCLUDE_PATH(include);
  }

  gSystem->SetBuildDir("build", kTRUE);

  gSystem->Load("libGpad.so");
  gSystem->Load("libSQLIO.so");
  gSystem->Load("libTree.so");
  gSystem->Load("lib/libVME.so");
  gSystem->Load("lib/libMonitor.so");
  gSystem->Load("lib/libStrela.so");

  // with explicitly linking the dependent libraries
  // R__LOAD_LIBRARY(lib/libVME);
  // R__LOAD_LIBRARY(lib/libMonitor);
  // R__LOAD_LIBRARY(lib/libStrela);

  gStyle->SetGridColor(kGray);
  gStyle->SetPalette(55);
}
