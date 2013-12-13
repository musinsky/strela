// Author: Jan Musinsky
// 04/12/2013

#ifndef __CINT__
#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>

void rootlogon()
#endif
{
  if (gROOT->GetClass("TStrela")) {
    Printf("Strela libraries was already loaded");
    return;
  }

  gSystem->AddIncludePath("-Iinclude");
  gSystem->SetBuildDir("build", kTRUE);

  gSystem->Load("libGpad");
  gSystem->Load("libSQLIO");
  gSystem->Load("libTree");
  gSystem->Load("lib/libVME.so");
  gSystem->Load("lib/libStrela.so");

  gStyle->SetHistLineColor(kBlack);
  gStyle->SetGridColor(kGray);
}
