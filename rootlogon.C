// Author: Jan Musinsky
// 15/09/2011

{
  if (gROOT->GetClass("TStrela")) {
    Printf("Strela libraries was already loaded");
    return;
  }

  gSystem->AddIncludePath("-Iinclude");
  gSystem->SetBuildDir("tmp", kTRUE);

  gSystem->Load("libGpad");
  gSystem->Load("libSQLIO");
  gSystem->Load("lib/libVME.so");
  gSystem->Load("lib/libStrela.so");

  gStyle->SetHistLineColor(kBlack);
  gStyle->SetGridColor(kGray+2);
  gStyle->SetOptDate(21);
  gStyle->SetDateX(0.005);
  gStyle->SetDateY(0.005);
  gStyle->GetAttDate()->SetTextSize(0.015);
}
