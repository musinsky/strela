// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    28 Nov 2016

#include <THttpServer.h>
#include <TROOT.h>
#include <TFolder.h>
#include <TStyle.h>
#include <TMultiGraph.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TRootSniffer.h>
#include <TH1.h>

#include "THttpMonitor.h"
#include "TVME.h"
#include "TVMERawData.h"
#include "TStrela.h"
#include "TStrawCham.h"
#include "TStrawTube.h"

ClassImp(THttpMonitor)

//______________________________________________________________________________
THttpMonitor::THttpMonitor()
: TObject(),
  fHttpServer(0),
  fIconPath(),
  fDatime(),
  fListMSCs(0),
  fMultiPolars(0),
  fMultiChambers(0),
  fSpills(0),
  fEvents(0)
{
  // Default constructor
}
//______________________________________________________________________________
THttpMonitor::THttpMonitor(Int_t port, const char *topname, const char *httptitle)
: TObject(),
  fHttpServer(0),
  fIconPath(),
  fDatime(),
  fListMSCs(0),
  fMultiPolars(0),
  fMultiChambers(0),
  fSpills(0),
  fEvents(0)
{
  // Normal constructor
  fHttpServer = new THttpServer(TString::Format("http:%d?top=%s", port, topname));
  fIconPath = "rootsys/icons/strela/";
  if (gVME) {
    gVME->RegisterHttpMonitor(this);
    fHttpServer->RegisterCommand("/SwitchResetCycle", "gVME->SwitchResetCycle()", fIconPath + "recycle_black.svg");
    fHttpServer->RegisterCommand("/Reset", "gVME->ResetMonitors(\"%arg1%\")", fIconPath + "reset.svg");
  }

  // monitor log
  fHttpServer->CreateItem("/Log", "log info");
  fHttpServer->SetItemField("/Log", "_kind", "Text");
  //  fHttpServer->SetItemField("/Log", "_icon", fIconPath + "log.svg");
  fHttpServer->SetItemField("/Log", "value", "log data");

  TFolder *rootfold = gROOT->GetRootFolder();
  TFolder *httpfold = dynamic_cast<TFolder *>(rootfold->FindObject("http"));
  if (httpfold) httpfold->SetTitle(httptitle); // don't change "http" name

  fDatime.Set();
  gStyle->SetTimeOffset(fDatime.Convert());
}
//______________________________________________________________________________
THttpMonitor::~THttpMonitor()
{
  // Destructor
  if (fListMSCs) {
    fListMSCs->Delete();
    delete fListMSCs;
    fListMSCs = 0;
  }
  SafeDelete(fMultiPolars);
  SafeDelete(fMultiChambers);
  SafeDelete(fHttpServer);
}
//______________________________________________________________________________
void THttpMonitor::SetDefaultDraw(const char *item, const char *layout, const char *monitor)
{
  if (!fHttpServer) return;

  fHttpServer->SetItemField("/", "_drawitem", item);
  fHttpServer->SetItemField("/", "_layout", layout);
  fHttpServer->SetItemField("/", "_monitoring", monitor); // 3000 is default
}
//______________________________________________________________________________
void THttpMonitor::SwitchResetCycle()
{
  Bool_t prev = IsResetCycle();
  ResetCycle(!prev);
  Printf("SwitchResetCycle = %d", IsResetCycle());
}
//______________________________________________________________________________
void THttpMonitor::ResetMonitors(const char *arg1)
{
  if (strcmp(arg1, "yes") != 0) return;

  Printf("ResetMonitors");
  fSpills = 0;
  fEvents = 0;
  if (fListMSCs) ResetGraphs(fListMSCs);
  if (fMultiPolars) ResetGraphs(fMultiPolars->GetListOfGraphs());
  if (fMultiChambers) ResetGraphs(fMultiChambers->GetListOfGraphs());
}
//______________________________________________________________________________
TGraph *THttpMonitor::MakeGraph(const char *gn, const char *gt, Style_t ms, Style_t ls, Color_t c) const
{
  TGraph *g = new TGraph();
  g->SetName(gn);
  g->SetTitle(gt);
  g->SetMarkerStyle(ms);
  g->SetMarkerColor(c);
  g->SetLineStyle(ls);
  g->SetLineColor(c);
  g->GetXaxis()->SetNdivisions(407);
  g->SetPoint(0, fDatime.Convert() - gStyle->GetTimeOffset(), 0.);

  return g;
}
//______________________________________________________________________________
void THttpMonitor::ResetGraphs(const TList *list) const
{
  if (!list) return;

  TGraph *g;
  TIter next(list);
  while ((g = (TGraph *)next()))
    g->Set(0);
}
//______________________________________________________________________________
void THttpMonitor::AddGraphPoint(TGraph *g, Double_t value) const
{
  if (!g) return;

  g->SetPoint(g->GetN(), fDatime.Convert() - gStyle->GetTimeOffset(), value);
  g->GetXaxis()->SetTimeDisplay(1);
  g->GetXaxis()->SetTimeFormat("%H:%M:%S");
}
//______________________________________________________________________________
void THttpMonitor::MonitorMSC(Int_t nmsc, const char *subfold)
{

  // ale na druhej strane MonitorStraTube cez gStrela

  if (!fHttpServer) return;
  if (nmsc == 0) {
    Warning("MonitorMSC", "no MSC counters");
    return;
  }
  if (fListMSCs) {
    Warning("MonitorMSC", "MSC monitor already initialized");
    return;
  }

  fListMSCs = new TList();
  TGraph *g;
  for (Int_t i = 0; i < nmsc; i++) {
    g = MakeGraph(TString::Format("MSC%02d", i + 1), TString::Format("MSC[%02d]", i + 1), 7, 3, kBlack);
    fListMSCs->Add(g);
    fHttpServer->Register(subfold, g);
  }
}
//______________________________________________________________________________
void THttpMonitor::MonitorPolar(const char *subfold)
{
  if (!fHttpServer) return;
  if (fMultiPolars) {
    Warning("MonitorPolar", "polar monitor already initialized");
    return;
  }

  fMultiPolars = new TMultiGraph("pol", "pol = (Left-Right) / IonCham");
  TGraph *g;
  g = MakeGraph("polZ", "pol Zero", 7, 3, kRed);
  fMultiPolars->Add(g);
  g = MakeGraph("polP", "pol Plus", 7, 3, kGreen);
  fMultiPolars->Add(g);
  g = MakeGraph("polM", "pol Minus", 7, 3, kBlue);
  fMultiPolars->Add(g);

  fHttpServer->Register(subfold, fMultiPolars);
}
//______________________________________________________________________________
void THttpMonitor::MonitorChamber(const char *subfold)
{
  if (!fHttpServer) return;
  if (fMultiChambers) {
    Warning("MonitorChamber", "chamber monitor already initialized");
    return;
  }

  fMultiChambers = new TMultiGraph("cham", "small & big drift cham");
  TGraph *g;
  g = MakeGraph("dchS", "small drift cham", 7, 3, kBlue);
  fMultiChambers->Add(g);
  g = MakeGraph("dchB", "big drift cham", 7, 3, kGreen);
  fMultiChambers->Add(g);

  fHttpServer->Register(subfold, fMultiChambers);
}
//______________________________________________________________________________
void THttpMonitor::MonitorStrawTube(const char *subfold)
{
  if (!fHttpServer) return;
  if (!gStrela) {
    Warning("MonitorTStrawTube", "gSTRELA not initialized");
    return;
  }
  // ale na druhej strane MonitorMSC(Int_t nmsc) a nie cez gVME

  TStrawTube *tube;
  TIter next(gStrela->StrawCham()->Tubes());
  while ((tube = (TStrawTube *)next())) {
    if (fHttpServer->GetSniffer()->FindTObjectInHierarchy(
        TString::Format("%s/%s", subfold, tube->HisTime1()->GetName()))) continue;
    fHttpServer->Register(subfold, tube->HisTime1());
    fHttpServer->Register(subfold, tube->HisDelta());
  }
}
//______________________________________________________________________________
void THttpMonitor::MiningInfo(const TVMERawData *rd, const char *dn, Long_t df, Long_t dt)
{
  if (!fHttpServer) return;
  if (!rd) return;

  fSpills++;
  fEvents += rd->GetNEvents();

  TString log;
  log =  Form("total spills:\t %d \ntotal events:\t %ld \n \n", fSpills, fEvents);
  log += Form("recv time:\t %02d:%02d:%02d \n", fDatime.GetHour(), fDatime.GetMinute(), fDatime.GetSecond());
  log += Form("recv data:\t %s \nrecv bytes:\t %ld -> %ld (total %ld) \nrecv events:\t %d ",
              dn, df, dt, dt - df, rd->GetNEvents());
  fHttpServer->SetItemField("/Log", "value", log.Data());
}
//______________________________________________________________________________
void THttpMonitor::MiningMonitors(const TVMERawData *rd)
{
  if (!fHttpServer) return;
  if (!rd) return;
  TGraph *g;

  if (fListMSCs) {
    if (fListMSCs->GetSize() != rd->GetMSCCnt().GetSize()) {
      Warning("MiningMonitors", "different number of MSC counters");
    } else {
      for (Int_t i = 0; i < fListMSCs->GetSize(); i++) {
        g = dynamic_cast<TGraph *>(fListMSCs->At(i));
        AddGraphPoint(g, rd->GetMSCCnt().At(i));
        //        g->SetPoint(g->GetN(), fDatime.Convert() - gStyle->GetTimeOffset(), rd->GetMSCCnt().At(i));
        //        g->GetXaxis()->SetTimeDisplay(1);
        //        g->GetXaxis()->SetTimeFormat("%H:%M:%S");
      }
    }
  }

  if (fMultiPolars) {
    Long64_t zero, plus, minus, trig;
    zero  = rd->GetTree()->Draw("", "EventTdc.MultiTdc(57)", "goff");
    plus  = rd->GetTree()->Draw("", "EventTdc.MultiTdc(56)", "goff");
    minus = rd->GetTree()->Draw("", "EventTdc.MultiTdc(59)", "goff");
    trig  = rd->GetTree()->Draw("", "EventTdc.MultiTdc(58)", "goff");

    Int_t ig = -1;
    if      (TMath::Abs(trig*0.90 - zero)  < trig*0.11) ig = 0;
    else if (TMath::Abs(trig*0.90 - plus)  < trig*0.11) ig = 1;
    else if (TMath::Abs(trig*0.90 - minus) < trig*0.11) ig = 2;
    else                                                ig = -1;

    Double_t left  = rd->GetMSCCnt().At(5-1);
    Double_t right = rd->GetMSCCnt().At(6-1);
    Int_t    ionch = rd->GetMSCCnt().At(7-1);
    Double_t ratio = (left-right)/ionch;
    if(ionch == 0) ratio=0.;

    if (ig == -1) {
      Warning("MiningMonitors", "no polarization mode");
    } else {
      g = dynamic_cast<TGraph *>(fMultiPolars->GetListOfGraphs()->At(ig));
      AddGraphPoint(g, ratio);
      //      g->SetPoint(g->GetN(), fDatime.Convert() - gStyle->GetTimeOffset(), ratio);
      //      g->GetXaxis()->SetTimeDisplay(1);
      //      g->GetXaxis()->SetTimeFormat("%H:%M:%S");
    }
  }

  if (fMultiChambers) {
    g = dynamic_cast<TGraph *>(fMultiChambers->GetListOfGraphs()->At(0)); // small
    AddGraphPoint(g, rd->GetTree()->Draw("", "GetChannelTdc() < 31", "goff"));
    //    g->SetPoint(g->GetN(), fDatime.Convert() - gStyle->GetTimeOffset(),
    //                rd->GetTree()->Draw("", "GetChannelTdc() < 31", "goff"));
    //    g->GetXaxis()->SetTimeDisplay(1);
    //    g->GetXaxis()->SetTimeFormat("%H:%M:%S");

    g = dynamic_cast<TGraph *>(fMultiChambers->GetListOfGraphs()->At(1)); // big
    AddGraphPoint(g, rd->GetTree()->Draw("", "GetChannelTdc() > 63", "goff"));
    //    g->SetPoint(g->GetN(), fDatime.Convert() - gStyle->GetTimeOffset(),
    //                rd->GetTree()->Draw("", "GetChannelTdc() > 63", "goff"));
    //    g->GetXaxis()->SetTimeDisplay(1);
    //    g->GetXaxis()->SetTimeFormat("%H:%M:%S");
  }


}
