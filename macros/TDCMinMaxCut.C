// root[] .x Strela.C
// root[] .L macros/TDCMinMaxCut.C+
// root[] TStrawCham::Tracking(-1)
// root[] gStrela->StrawCham()->SetTubesTimes(0, 0, 9000)
// root[] gStrela->AnalyzeEntries()
//
// root[] TDCMinMaxCut()
// root[] TDCManual(); ZoomAxis()
// root[] ExportMinMax(); > seans_channels_updateTDCcut.sql

// Author: Jan Musinsky
// 19/11/2010

#include <TH1.h>
#include <TROOT.h>
#include <TF1.h>
#include <TFile.h>
#include <TLine.h>
#include <TCanvas.h>

#include "TStrawTracker.h"

const UInt_t id1 = 11, id2 = 22;
// const Double_t t1out = 3500, t2out = 9700; // must be out of TDC time
const Double_t t1out = 700, t2out = 7000; // must be out of TDC time 2009_12

TLine *FindLine(TH1F *histogram, UInt_t idline)
{
  if (!histogram) return 0;

  TIter next(histogram->GetListOfFunctions());
  TObject *obj;
  while ((obj = next()))
    if (obj->InheritsFrom(TLine::Class()) && (obj->GetUniqueID() == idline))
      return (TLine *)obj;
  return 0;
}

void FindTimeCut(TH1F *histo, Int_t range)
{
  if (!histo) return;

  Double_t c = histo->GetMaximum()/5;
  Int_t bin, bc = histo->FindBin(t1out + (t2out - t1out)/2);
  Int_t b1 = histo->FindBin(t1out);
  Int_t b2 = histo->FindBin(t2out);
  Int_t tt1, tt2, t1 = t1out, t2 = t2out;

  Double_t diff, maxdiff = 0;
  for (Int_t ib = (b1 + 1); ib < b2; ib++) {
    diff = TMath::Abs(histo->GetBinContent(ib) - histo->GetBinContent(ib-1));
    if (diff > maxdiff) maxdiff = diff;
  }

  Double_t del, maxdel = 1.e99;
  while (c > 0) {
    histo->GetBinWithContent(c, bin, b1, bc, maxdiff);
    tt1 = (Int_t)histo->GetBinCenter(bin);
    histo->GetBinWithContent(c, bin, bc, b2, maxdiff);
    tt2 = (Int_t)histo->GetBinCenter(bin);

    del = TMath::Abs((tt2 - tt1) - range);
    if (del < maxdel) {
      maxdel = del;
      t1 = tt1;
      t2 = tt2;
    }
    c--;
  }

  // add lines
  TLine *line1 = FindLine(histo, id1);
  if (!line1) {
    line1 = new TLine();
    line1->SetUniqueID(id1);
    line1->SetLineColor(kRed);
    histo->GetListOfFunctions()->Add(line1);
  }
  TLine *line2 = FindLine(histo, id2);
  if (!line2) {
    line2 = new TLine();
    line2->SetUniqueID(id2);
    line2->SetLineColor(kBlue);
    histo->GetListOfFunctions()->Add(line2);
  }

  Double_t ymin = histo->GetMinimum(), ymax = histo->GetMaximum();
  if (ymin == ymax) ymax += 1; // empty or corrupted histo
  line1->SetX1(t1); line1->SetX2(t1); line1->SetY1(ymin); line1->SetY2(ymax);
  line2->SetX1(t2); line2->SetX2(t2); line2->SetY1(ymin); line2->SetY2(ymax);

  histo->SetTitle(Form("t1 = %5d, t2 = %5d, range = %5d", t1, t2, t2 - t1));
  if (gDebug > 0) Printf("%s, t1 = %5d, t2 = %5d", histo->GetName(), t1, t2);
}

void TDCMinMaxCut(Int_t range = 4400, const char *fname = 0)
{
  if (!gStrela->StrawCham()) return;

  TFile *hfile = 0;
  if (fname) hfile = TFile::Open(fname);

  TIter next(gStrela->StrawCham()->Tubes());
  TStrawTube *tube;
  while ((tube = (TStrawTube *)next())) {
    if (hfile) {
      TH1F *fhis;
      hfile->GetObject(tube->HisTime1()->GetName(), fhis);
      if (!fhis) Printf("%s does not exist", tube->HisTime1()->GetName());
      else {
        tube->HisTime1()->Reset();
        tube->HisTime1()->Add(fhis);
        tube->HisTime1()->SetTitle(fhis->GetTitle());
      }
      hfile->GetObject(tube->HisTime2()->GetName(), fhis);
      if (!fhis) Printf("%s does not exist", tube->HisTime2()->GetName());
      else {
        tube->HisTime2()->Reset();
        tube->HisTime2()->Add(fhis);
        tube->HisTime2()->SetTitle(fhis->GetTitle());
      }
    }
    // need for log drawing
    tube->HisTime1()->SetMinimum();
    if (tube->HisTime1()->GetMinimum() < 1) tube->HisTime1()->SetMinimum(1);
    tube->HisTime2()->SetMinimum();
    if (tube->HisTime2()->GetMinimum() < 1) tube->HisTime2()->SetMinimum(1);

    FindTimeCut(tube->HisTime2(), range);
  }
  delete hfile;
}

void ExportMinMax()
{
  TIter trackers(gStrela->StrawCham()->Trackers());
  TStrawTracker *tracker;
  while ((tracker = (TStrawTracker *)trackers())) {
    Printf("/*%12s*/", tracker->GetName());
    TIter layers(tracker->Layers());
    TStrawLayer *layer;
    while ((layer = (TStrawLayer *)layers())) {
      TIter tubes(layer->Tubes());
      TStrawTube *tube;
      while ((tube = (TStrawTube *)tubes())) {
        printf("UPDATE %s.channels SET T0 = %5d, TMax = %5d",
               gStrela->GetSeance(),
               (Int_t)FindLine(tube->HisTime2(), id1)->GetX1(),
               (Int_t)FindLine(tube->HisTime2(), id2)->GetX1());
        printf(" WHERE channels.Nadc = %5d LIMIT 1; /*%10s*/\n",
               tube->GetNadc(), layer->GetName());
      }
    }
  }
}

void ZoomAxis(Double_t x1 = -1, Double_t x2 = -1)
{
  TIter next(gStrela->StrawCham()->Tubes());
  TStrawTube *tube;
  while ((tube = (TStrawTube *)next())) {
    tube->HisTime1()->GetXaxis()->SetRangeUser(x1, x2);
    tube->HisTime2()->GetXaxis()->SetRangeUser(x1, x2);
  }
}

void RebinCutHisto(Bool_t same = kFALSE, Int_t ng = 2)
{
  TIter next(gStrela->StrawCham()->Tubes());
  TStrawTube *tube;
  while ((tube = (TStrawTube *)next())) {
    if (same) {
      Int_t bw1 = tube->HisTime1()->GetBinWidth(0);
      Int_t bw2 = tube->HisTime2()->GetBinWidth(0);
      if (bw1 == bw2) continue;
      if (bw1 > bw2) tube->HisTime2()->Rebin(bw1/bw2);
      else           tube->HisTime1()->Rebin(bw2/bw1);
    }
    else {
      tube->HisTime1()->Rebin(ng);
      tube->HisTime2()->Rebin(ng);
    }
  }
}

//______________________________________________________________________________

void TDCManual(Bool_t enable = kTRUE)
{
  TCanvas *c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("c_tube");
  if (!c) {
    Printf("does not exist necessary canvas");
    return;
  }

  const char *name = "cut_exec";
  c->GetPad(2)->DeleteExec(name); // remove anytime
  if (!enable) return;
  c->GetPad(2)->AddExec(name, "cutexec()");
}

void ShiftLine(UInt_t idline, Double_t xnew)
{
  if (!gPad) return;

  TH1F *his = 0;
  TIter next(gPad->GetListOfPrimitives());
  TObject *obj;
  while ((obj = next())) {
    if (obj->InheritsFrom(TH1::Class())) {
      his = (TH1F *)obj;
      break;
    }
  }
  if (!his) return;

  if (idline == 0) {
    Printf("%s, %5d, %5d", his->GetName(), (Int_t)FindLine(his, id1)->GetX1(),
           (Int_t)FindLine(his, id2)->GetX1());
    return;
  }

  TLine *line = FindLine(his, idline);
  line->SetX1(xnew); line->SetX2(xnew);
  Int_t t1 = (Int_t)FindLine(his, id1)->GetX1();
  Int_t t2 = (Int_t)FindLine(his, id2)->GetX1();
  his->SetTitle(Form("t1 = %5d, t2 = %5d, range = %5d", t1, t2, t2 - t1));
  //  gPad->Modified();
  //  gPad->Update();
  gPad->GetCanvas()->Draw();
  gPad->GetCanvas()->Modified();
  gPad->GetCanvas()->Update();
}

void cutexec()
{
  Int_t event = gPad->GetEvent();
  Int_t px = gPad->GetEventX();
  static Double_t xlast;

  if(event != kKeyPress) {
    xlast = gPad->AbsPixeltoX(px);
    return;
  }

  switch (px) {
  case '1':
    ShiftLine(id1, xlast);
    return;
  case '2':
    ShiftLine(id2, xlast);
    return;
  case 's':
    ShiftLine(0, 0);
    return;
  }
}
