// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 17 Jun 2008

#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TMath.h>
#include <TH1.h>

#include "TStrawCham.h"
#include "TStrawTracker.h"
#include "TGemEvent.h"
#include "TStrawMulti.h"
#include "TVME.h"

Int_t TStrawCham::fgTrigNadc = -1;
Int_t TStrawCham::fgShiftAdc = 0;
Int_t TStrawCham::fgTracking = 1;
Int_t TStrawCham::fgIter     = 0;

ClassImp(TStrawCham)

//______________________________________________________________________________
TStrawCham::TStrawCham()
{
  //  Info("TStrawCham", "Default constructor");
  fTrackers  = 0;
  fLayers    = 0;
  fMulties   = 0;
  fTubes     = 0;
  fTubesI    = 0;
}
//______________________________________________________________________________
TStrawCham::TStrawCham(const char *name, const char *title)
  : TStrelaBase(name, title)
{
  //  Info("TStrawCham", "Normal constructor");
  fTrackers   = 0;
  fLayers     = 0;
  fMulties    = 0;
  fTubes      = 0;
  fTubesI     = 0;
  fContainer  = new TClonesArray("TStrawTrack", 50);
  fBranchName = "tracks";
}
//______________________________________________________________________________
TStrawCham::~TStrawCham()
{
  Info("~TStrawCham", "Destructor");
  DeleteArrays();
}
//______________________________________________________________________________
void TStrawCham::Print(Option_t* option) const
{
  if (!fLayers) return;
  TStrawTracker *tracker = 0;
  TStrawLayer *layer;
  TStrawTube *tube;
  TIter nextLayer(fLayers);
  while((layer = (TStrawLayer *)nextLayer())) {
    if (tracker != layer->GetTracker()) {
      tracker = layer->GetTracker();
      Printf("=======> %s <=======", tracker->GetName());
    }
    Printf("[%2d] %s", layer->GetNumb(), layer->GetName());
    if (strcmp(option, "all") || !fTubes) continue;

    for (Int_t i = 0; i < layer->Tubes()->GetSize(); i++) {
      tube = (TStrawTube *)layer->Tubes()->At(i);
      Printf(" %2d) %s %8.2f %8.2f (%3d)", i + 1,
             gVME->GetChannelInfo(tube->GetNadc()),
             tube->GetCenter(), tube->GetZ(), fTubes->BinarySearch(tube));
    }
  }
  Printf("\ntrig  %s", gVME->GetChannelInfo(fgTrigNadc));
}
//______________________________________________________________________________
void TStrawCham::DeleteArrays()
{
  if (fTrackers) fTrackers->Delete();
  delete fTrackers;
  fTrackers = 0;
  if (fMulties) fMulties->Delete();
  delete fMulties;
  fMulties = 0;
  if (fLayers) fLayers->Delete(); // at last delete (recursive) tubes & layers
  delete fLayers;
  fLayers = 0;
  delete fTubes;
  fTubes = 0;
  delete [] fTubesI;
}
//______________________________________________________________________________
Bool_t TStrawCham::ReadSQL(TSQLServer *ser)
{
  DeleteArrays();
  Clear();
  const char *table;
  TSQLResult *res;
  TSQLRow *row;
  TStrawTracker *tracker = 0;
  TStrawLayer *layer = 0;
  TStrawTube *tube = 0;
  Int_t ntubes = 0;

  // make trackers, layers, tubes
  table = "layers";
  res = ser->Query(Form("SELECT `Tracker`, `Numb`, `Name`, `NChan`, `Delta`, `Direct`, `Z`, `Shift`, `Range` FROM `%s` WHERE `Tracker` > 0 ORDER BY `Tracker`, `Numb`", table));
  // important only "ORDER BY Tracker"

  if (res) {
    if (res->GetRowCount() > 0) {
      fTrackers = new TList();
      fLayers = new TList();
      Int_t newId = 0; // "WHERE Tracker > 0"
      while ((row = res->Next())) {
        // make trackers
        if (newId != atoi(row->GetField(0))) {
          newId = atoi(row->GetField(0));
          tracker = new TStrawTracker(newId);
          fTrackers->Add(tracker);
        }
        // make layers, tubes
        layer = new TStrawLayer(atoi(row->GetField(1)), row->GetField(2));
        fLayers->Add(layer);
        layer->MakeTubes(atoi(row->GetField(3)), atof(row->GetField(4)),
                         atoi(row->GetField(5)));
        layer->SetZ(atof(row->GetField(6)));
        layer->Shift(atof(row->GetField(7)));
        layer->SetRange(atof(row->GetField(8)));
        layer->SetTracker(tracker);
        layer->SetTitle(Form("from %s", tracker->GetName()));
        ntubes += layer->Tubes()->GetSize();
        delete row;
      }
    }
    delete res;
  }

  if (!fTrackers) {
    Info("ReadSQL", "working without any straws");
    delete ser;
    return kFALSE;
  }

  // set tubes properties
  table = "channels";
  fMulties = new TList(); // "manually" add tubes
  fTubes = new TObjArray(ntubes);
  TIter next(fLayers);
  while ((layer = (TStrawLayer *)next())) {
    for (Int_t i = 0; i < layer->Tubes()->GetSize(); i++) {
      tube = (TStrawTube *)layer->Tubes()->At(i);
      fTubes->Add(tube); // order is not imoportant, will be sorted
      res = ser->Query(Form("SELECT `Nadc`, `T0`, `TMax` FROM `%s` WHERE `DetNumb` = %d AND `DetChan` = %d", table, layer->GetNumb(), i+1));
      if (res) {
        Int_t nchan = res->GetRowCount();
        if (nchan == 1) {
          row = res->Next();
          tube->SetNadc(atoi(row->GetField(0)));
          tube->SetTime(atoi(row->GetField(1)), atoi(row->GetField(2)));
          delete row;
        }
        else if (nchan == 0)
          Warning("ReadSQL", "channel %02d, %s(%02d) %s does not exist",
                  i+1, layer->GetName(), layer->GetNumb(), layer->GetTitle());
        else
          Warning("ReadSQL", "channel %02d, %s(%02d) %s is not unique",
                  i+1, layer->GetName(), layer->GetNumb(), layer->GetTitle());
        delete res;
      }
    }
  }
  delete ser;

  fTubes->Sort(); // sort by increasing Nadc
  if (fTubes->GetEntries() != fTubes->GetEntriesFast()) {
    Error("ReadSQL", "problem with fTubes entries/size");
    return kFALSE;
  }
  ntubes = fTubes->GetEntriesFast();
  fTubesI = new Int_t[ntubes];
  for (Int_t i = 0; i < ntubes; i++)
    fTubesI[i] = GetTube(i)->GetNadc();

  TIter nextt(fTrackers);
  while ((tracker = (TStrawTracker *)nextt()))
    tracker->AllocateLayers();

  return kTRUE;
}
//______________________________________________________________________________
TStrawTube *TStrawCham::SearchTube(Int_t nadc) const
{
  Int_t pos = TMath::BinarySearch(fTubes->GetEntriesFast(), fTubesI, nadc);
  if ((pos >= 0) && (GetTube(pos)->GetNadc() == nadc)) return GetTube(pos);
  //  Error("SearchTube", "nadc %4d not found", nadc);
  return 0;
}
//______________________________________________________________________________
void TStrawCham::SetTubesTime(Int_t del, Int_t t1, Int_t t2) const
{
  Int_t ntubes = fTubes->GetEntriesFast();

  static Bool_t first = kTRUE;
  static TArrayI defaultMin(ntubes), defaultMax(ntubes);
  if (first) {
    for (Int_t i = 0; i < ntubes; i++) {
      defaultMin[i] = GetTube(i)->GetTMin();
      defaultMax[i] = GetTube(i)->GetTMax();
    }
    first = kFALSE;
  }

  TStrawTube *tube;
  Int_t prevMin, prevMax;
  for (Int_t i = 0; i < ntubes; i++) {
    tube = GetTube(i);
    prevMin = tube->GetTMin();
    prevMax = tube->GetTMax();
    if      (del == 1) tube->SetTime(prevMin + t1, prevMax + t2);
    else if (del == 0) tube->SetTime(t1, t2);
    else               tube->SetTime(defaultMin[i], defaultMax[i]);
    if (gDebug > 0)
      Printf("%s\t tmin = %4d, tmax = %4d",
             tube->GetName(), tube->GetTMin(), tube->GetTMax());
  }
}
//______________________________________________________________________________
void TStrawCham::SetTubesCutTime(Option_t *option, Int_t t1, Int_t t2) const
{
  TString opt = option;
  TStrawTube *tube;

  for (Int_t i = 0; i < fTubes->GetEntriesFast(); i++) {
    tube = GetTube(i);
    if      (opt == "tmin")
      tube->SetCutTime(tube->GetTMin() + t1, tube->GetTMin() + t2);
    else if (opt == "tmax")
      tube->SetCutTime(tube->GetTMax() + t1, tube->GetTMax() + t2);
    else
      tube->SetCutTime(t1, t2);
    if (gDebug > 0)
      Printf("%s\t cutt1 = %4d, cutt2 = %4d",
             tube->GetName(), tube->GetCutT1(), tube->GetCutT2());
  }
}
//______________________________________________________________________________
void TStrawCham::AnalyzeBegin()
{
  ;
}
//______________________________________________________________________________
void TStrawCham::AnalyzeEntry()
{
  Clear();
  TIter next(fTrackers);
  TStrawTracker *tracker;
  while ((tracker = (TStrawTracker *)next()))
    tracker->ResetHits();

  TGemEvent *gemEvent = gStrela->GemEvent();
  TAdcHit1 *adcHit1;
  Int_t nadc, adc, pos, trigAdc = 0; // must be 0
  TStrawTube *tube;

  // first find trigger tdc (quick), in mostly cases trigger is first hit
  if (fgTrigNadc > 0) {
    Bool_t findTrig = kFALSE;
    for (Int_t ih = 0; ih < gemEvent->GetNumOfAdcHits1(); ih++) {
      adcHit1 = gemEvent->GetAdcHit1(ih);
      if (adcHit1->GetNadc() == fgTrigNadc) {
        trigAdc = adcHit1->GetAdc() - fgShiftAdc;
        findTrig = kTRUE;
        break;
      }
    }
    if (!findTrig) {
      Warning("AnalyzeEntry", "not found trigger hit");
      Printf(gStrela->GetEventInfo());
      return;
    }
  }

  for (Int_t ih = 0; ih < gemEvent->GetNumOfAdcHits1(); ih++) {
    adcHit1 = gemEvent->GetAdcHit1(ih);
    nadc    = adcHit1->GetNadc();
    adc     = adcHit1->GetAdc();
    pos     = TMath::BinarySearch(fTubes->GetEntriesFast(), fTubesI, nadc);
    if (pos < 0) continue;
    tube    = GetTube(pos);
    if (nadc != tube->GetNadc()) continue;
    // substract trigAdc (only if exist, otherwise adc without change)
    adc    -= trigAdc;
    tube->HisTime()->Fill(adc);
    // t0 can be tmin or tmax
    if ((adc < tube->GetTMin()) || (adc > tube->GetTMax())) continue;
    tube->GetTracker()->AddHit(pos, TMath::Abs(adc - tube->GetT0()));
  }
  if (fgTracking == 0) return;

  next.Reset();
  if (fgTracking == -1)
    while ((tracker = (TStrawTracker *)next()))
      tracker->TubesCutTimeInterval();
  else
    while ((tracker = (TStrawTracker *)next()))
      tracker->FindTracks();
}
//______________________________________________________________________________
void TStrawCham::AnalyzeTerminate()
{
  EfficiencyTubes();
}
//______________________________________________________________________________
void TStrawCham::IterNext(Int_t ne)
{
  if (!fMulties || fMulties->IsEmpty()) {
    Warning("IterFirst", "working without any multies");
    return;
  }

  gStrela->HistoManager("tube_*", "reset");
  gStrela->HistoManager("tracker_*", "reset");
  if ((ne == 0) || (ne > gStrela->GetEntries())) ne = gStrela->GetEntries();
  Printf("Iteration number = %2d, entries = %d", fgIter, ne);
  for (Int_t i = 0; i < ne; i++) {
    gStrela->GetChain()->GetEntry(i);
    AnalyzeEntry();
  }

  TIter next(fMulties);
  TStrawMulti *multi;
  while ((multi = (TStrawMulti *)next()))
    multi->IterNext();

  fgIter++;
}
//______________________________________________________________________________
void TStrawCham::EfficiencyTubes() const
{
  TIter next(fTubes);
  TStrawTube *tube;
  while ((tube = (TStrawTube *)next())) {
    tube->HisEffi()->Divide(tube->HisDis1(), tube->HisDis2(), 1, 1, "b");
    tube->HisEffi()->SetMaximum(1.1);
    tube->HisEffi()->SetMinimum(0.0);
    tube->HisEffi()->SetStats(kFALSE);
  }
}
