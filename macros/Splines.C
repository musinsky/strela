// Author: Jan Musinsky
// 14/12/2010

/*
  .L macros/Splines.C+
  SplineLinear()
  rad[3] = rad[2] = 2.1000
  rad[0] = rad[1] = 0.0025
  tmin = 975; tmax = 975+4450;
  SplineLinear()
  WriteFromPad("splines.root", "m1_spline")
  WriteFromPad("splines.root", "m2_spline")
*/

#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <TSpline.h>
#include <TCanvas.h>
#include <TLine.h>

const Int_t nreg = 3; // (begin, Tmin), (Tmin, Tmax), (Tmax, end)
Int_t tmin = 1000, tmax = 5500;
Double_t rad[nreg+1] = {0.0025/4., 0.0025/2., 2.10*1.00, 2.10*1.05};

void CreateSpline(TH1 *his)
{
  if (!his) return;

  const Int_t nb = his->GetNbinsX();
  Double_t x[nb], y[nb], sum = 0;
  for (Int_t i = 0; i < nb; i++) {
    x[i] = his->GetBinCenter(i+1);
    sum += his->GetBinContent(i+1);
    y[i] = sum;
  }

  Int_t index[nreg+1] = {0, his->FindBin(tmin), his->FindBin(tmax), nb};
  Double_t y_begin, y_end, ratio;

  for (Int_t reg = 0; reg < nreg; reg++) {
    y_begin = y[index[reg]];
    y_end   = y[index[reg+1]-1];
    ratio   = (rad[reg+1] - rad[reg])/(y_end - y_begin);
    for (Int_t i = index[reg]; i < index[reg+1]; i++)
      y[i] = (y[i] - y_begin)*ratio + rad[reg];
  }

  TSpline3 *spline = new TSpline3("", x, y, nb);
  spline->SetName("spline");
  spline->SetTitle(Form("T: %4d, %4d, %4d, %4d   R: %.5f, %.5f, %.5f, %.5f",
                        (Int_t)x[0], tmin, tmax, (Int_t)x[nb-1],
                        rad[0], rad[1], rad[2], rad[3]));
  spline->SetLineColor(kBlue);
  spline->SetNpx(1000);
  TGraph *gra = new TGraph(nb, x, y);
  gra->SetMarkerColor(kRed);
  his->Scale(rad[2]/his->GetMaximum());
  spline->Draw();
  gra->Draw("P");
  his->Draw("same");
  TLine line;
  line.SetLineStyle(2);
  line.DrawLine(tmin, rad[0], tmin, rad[3]);
  line.DrawLine(tmax, rad[0], tmax, rad[3]);
  line.SetLineStyle(5);
  line.SetLineColor(kGray+1);
  line.DrawLine(x[0], 2.1, x[nb-1], 2.1);
  if (gPad) gPad->SetGrid();
}

void SplineLinear(Int_t nb = 300, Double_t tlow = 0, Double_t tup = 9000)
{
  TH1F *htmp = (TH1F *)gROOT->FindObject("htmp_spline");
  if (htmp) delete htmp;
  htmp = new TH1F("htmp_spline", "", nb, tlow, tup);
  for (Int_t i = 0; i < htmp->GetNbinsX(); i++) {
    if ((i < htmp->FindBin(tmin)) || (i > htmp->FindBin(tmax)))
      htmp->SetBinContent(i+1, 5);
    else
      htmp->SetBinContent(i+1, 100);
  }
  CreateSpline(htmp);
}

void WriteFromPad(const char *fname, const char *sname)
{
  if (!gPad) return;
  TFile *file = TFile::Open(fname, "UPDATE");
  if (!file) return;
  TSpline3 *s = (TSpline3 *)gPad->FindObject("spline");
  if (!s) {
    Printf("unable to find spline object in gPad");
    delete file;
    return;
  }
  s->Write(sname, TObject::kOverwrite);
  Printf("%s", s->GetTitle());
  Printf("spline %s was written to %s", sname, file->GetName());
  delete file;
}

// void SplineIntegral(const char *fname, const char *hname)
// {
//   TFile *file = TFile::Open(fname);
//   if (!file) return;
//   TH1 *histo;
//   file->GetObject(hname, histo);
//   if (!histo) {
//     Printf("wrong histo name: %s", hname);
//     delete file;
//     return;
//   }
//   histo->SetDirectory(0);
//   CreateSpline(histo);
//   delete file;
// }
