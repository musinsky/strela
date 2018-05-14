// Author: Jan Musinsky
// 11/05/2018

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TMultiGraph.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TCanvas.h"

Int_t ch56, ch57, ch58, ch59;
TArrayI *MSC = 0; // must be 0
Int_t pol;
Double_t leftF3, rightF3, icF3;

Int_t FindPolarization();
void ForPiskunov(TString ffnn);

void polar()
{
  //  TFile *f = new TFile("run078.root", "READ");
  TChain *pp2 = new TChain("pp2");
  for (Int_t r = 39; r < 115; r++) { // 39 -> 114(115) // good after 50
    //    if (r == 97) continue;
    //    if (r == 98) continue;
    //    if (r == 99) continue;
    pp2->Add(TString::Format("/mnt/free2/root_2017_02/run%03d.root", r).Data());
  }
  //  TTree *pp2 = (TTree *)f->Get("pp2");
  if (!pp2) return;

  pp2->SetBranchAddress("ch56", &ch56);
  pp2->SetBranchAddress("ch57", &ch57);
  pp2->SetBranchAddress("ch58", &ch58);
  pp2->SetBranchAddress("ch59", &ch59);
  pp2->SetBranchAddress("MSC", &MSC);

  Int_t ms = 7;
  TMultiGraph *mg = new TMultiGraph("pola", "polarization");
  TGraphErrors *g = new TGraphErrors();
  g->SetName("polM");
  g->SetTitle("polM");
  g->SetMarkerStyle(ms);
  g->SetMarkerColor(kRed);
  g->SetLineColor(g->GetMarkerColor());
  g->SetLineStyle(3);
  mg->Add(g);

  g = new TGraphErrors();
  g->SetName("polP");
  g->SetTitle("polP");
  g->SetMarkerStyle(ms);
  g->SetMarkerColor(kGreen);
  g->SetLineColor(g->GetMarkerColor());
  g->SetLineStyle(3);
  mg->Add(g);

  g = new TGraphErrors();
  g->SetName("polZ");
  g->SetTitle("polZ");
  g->SetMarkerStyle(ms);
  g->SetMarkerColor(kBlue);
  g->SetLineColor(g->GetMarkerColor());
  g->SetLineStyle(3);
  mg->Add(g);

  TGraph *gr = new TGraph();
  gr->SetName("runs");
  gr->SetTitle("runs");
  gr->SetMarkerStyle(6);
  gr->SetMarkerColor(kGray+2);
  gr->SetLineColor(g->GetMarkerColor());
  gr->SetLineStyle(3);
  mg->Add(gr);

  TH1F *hhh = 0;
  Double_t xxmin = 0.0, xxmax = 1.;
  Int_t xxbin = 100;
  TH1F *h1 = new TH1F("h1","hM", xxbin, xxmin, xxmax);
  h1->SetLineColor(kRed);
  TH1F *h2 = new TH1F("h2","hP", xxbin, xxmin, xxmax);
  h2->SetLineColor(kGreen);
  TH1F *h3 = new TH1F("h3","hZ", xxbin, xxmin, xxmax);
  h3->SetLineColor(kBlue);

  Int_t prevPol = -123;
  TString fnc(0);
  for (Int_t i = 0; i < pp2->GetEntries(); i++) {
    pp2->GetEntry(i);

    if (fnc.CompareTo(pp2->GetCurrentFile()->GetName()) != 0) {
      //      Printf("NEWWWWWW FILE");
      fnc = pp2->GetCurrentFile()->GetName();
      prevPol = -123;
      gr->SetPoint(gr->GetN(), i, 0.5); // mark as next run
    }
    //    ForPiskunov(fnc);

    if (MSC->GetSize() != 16) Printf("WRONG MSC SIZE !!!");
    leftF3 =  MSC->At(5-1);
    rightF3 = MSC->At(6-1);
    icF3 =    MSC->At(7-1);
    if (icF3 == 0) {
      Printf("WRONG IC = 0 !!!");
      continue;
    }
    pol = FindPolarization();
    if (pol == -9) {
      Printf("WRONG POL !!!");
      continue;
    }
    //    Printf("pol = [%03d] %d", i, pol);

    Bool_t polCircular = kFALSE;
    if (prevPol != -123) {
      // circulation always: pol+ => pol0 => pol- ===>  1, 0, -1
      Int_t delPol = prevPol - pol;
      // circulation always: delpol -2 => 1 => 1
      //      Printf("delpol = %d", delPol);
      if (delPol == 1) polCircular = kTRUE;
      else if ((delPol == -2) && (prevPol == -1)) polCircular = kTRUE;
    } else
      polCircular = kTRUE;
    prevPol = pol;
    if (!polCircular) Printf("WRONG POL circulation !!!");
    // pozor pri TChain treba checknut ako keby od zaciatku !!!

    if      (pol == -1) {hhh = h1; g = (TGraphErrors *)mg->GetListOfGraphs()->At(0);} // pol MINUS
    else if (pol ==  1) {hhh = h2; g = (TGraphErrors *)mg->GetListOfGraphs()->At(1);} // pol PLUS
    else if (pol ==  0) {hhh = h3; g = (TGraphErrors *)mg->GetListOfGraphs()->At(2);} // pol ZERO
    else {
      Printf("WRONG no graph !!!");
      continue;
    }

    // if (icF3 < 50) continue; // skip wrong spill
    Double_t value1 = (leftF3 - rightF3)/icF3;
    //    Double_t value1error = TMath::Sqrt(leftF3)/leftF3 + TMath::Sqrt(rightF3)/rightF3;
    //    Double_t value2 = (leftF3 + rightF3)/icF3;
    Int_t iii = g->GetN();
    g->SetPoint(iii, i, value1);
    //    g->SetPointError(iii, 0.1, value1error);

    //    hhh->Fill(value1);
  }

  mg->Draw("AP");
  return;
  //  TCanvas *c1 = new TCanvas();
  //  c1->Divide(3, 1, 0.001, 0.001);
  //  c1->cd(1);
  //  h1->Draw();
  //  c1->cd(2);
  //  h2->Draw();
  //  c1->cd(3);
  //  h3->Draw();
}

Int_t FindPolarization()
{
  // none  => -9
  // ch56, pol - => -1
  // ch57, pol 0 =>  0
  // ch59, pol + => +1

  if (ch58 == 0) return -9;
  if ((ch56 == 0) && (ch57 == 0) && (ch59 == 0)) return -9;
  if ((ch56 != 0) && (ch57 == 0) && (ch59 == 0)) return -1;
  if ((ch56 == 0) && (ch57 != 0) && (ch59 == 0)) return 0;
  if ((ch56 == 0) && (ch57 == 0) && (ch59 != 0)) return 1;

  // treba este chceck ci ta hodnota, ktora sa nerovna 0 je blizka hodnote ch58
  return -9;
}

void ForPiskunov(TString ffnn)
{
  return;
  static FILE *txtfile = 0;
  static TString cccfn(0);
  if (ffnn.CompareTo(cccfn) != 0) {
    cccfn = ffnn;
    if (txtfile) fclose(txtfile);
    //txtfile = fopen(ffnn.Replace(7, 4, "txt"), "w");
    txtfile = fopen(ffnn.Replace(31, 4, "txt"), "w");
    Printf("!!!!!!!!!!!!! %s", ffnn.Data());
  }
  if (!txtfile) {
    Printf("NEMZE BYT, NIE JE FILE !!!");
    return;
  }

  fprintf(txtfile, "%d, %d, %d, %d, %d, ", ch56, ch57, ch58, ch59, FindPolarization());
  for (Int_t i=0;i<16;i++) {
    fprintf(txtfile, "%d", MSC->At(i));
    if (i == 15)
      fprintf(txtfile, "\n");
    else
      fprintf(txtfile, ", ");
  }
}
