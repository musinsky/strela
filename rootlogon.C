// Author: Jan Musinsky
// 20/07/2010

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

  gROOT->SetStyle("Plain"); // Default
  gStyle->SetGridColor(kGray + 2);
  gStyle->SetOptStat();     // 000001111
  gStyle->SetOptFit();      // 0111
  gStyle->SetStatW();       // 0.19
  gStyle->SetStatH();       // 0.10
  gStyle->SetStatFormat();  // 6.4g
  gStyle->SetOptDate(11);   // 1
  gStyle->SetDateX(0.005);  // 0.01
  gStyle->SetDateY(0.005);  // 0.01
  gStyle->GetAttDate()->SetTextFont(42);
  gStyle->GetAttDate()->SetTextSize(0.015);
}
