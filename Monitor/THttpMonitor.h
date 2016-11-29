// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    28 Nov 2016

#ifndef STRELA_THttpMonitor
#define STRELA_THttpMonitor

#include <TDatime.h>

class THttpServer;
//class TDatime;
class TMultiGraph;
class TGraph;
class TVMERawData;

class THttpMonitor : public TObject {

public:
  // status bits
  enum {
    kResetCycle  = BIT(14) // reset every cycle
  };

  THttpMonitor();
  THttpMonitor(Int_t port, const char *topname = "STRELA", const char *httptitle = "STRELA monitor");
  virtual ~THttpMonitor();

  THttpServer  *GetHttpServer() const { return fHttpServer; }
  const char   *GetIconPath() const { return fIconPath.Data(); }
  void          SetIconPath(const char *path) { fIconPath = path; }
  TDatime       GetDatime() const { return fDatime; }
  void          SetDatime() { return fDatime.Set(); }
  TList        *GetListMSCs() const { return fListMSCs; }
  TMultiGraph  *GetMultiPolars() const { return fMultiPolars; }
  TMultiGraph  *GetMultiChambers() const { return fMultiChambers; }
  Bool_t        IsResetCycle() const { return TestBit(kResetCycle); }
  void          ResetCycle(Bool_t set) { SetBit(kResetCycle, set); }

  void          SetDefaultDraw(const char *item, const char *layout = "grid2x2", const char *monitor = "3000");
  void          SwitchResetCycle();
  void          ResetMonitors(const char *arg1);
  void          MonitorMSC(Int_t nmsc, const char *subfold = "/");
  void          MonitorPolar(const char *subfold = "/");
  void          MonitorChamber(const char *subfold = "/");
  void          MonitorStrawTube(const char *subfold = "/StrawTube");

  void          MiningInfo(const TVMERawData *rd, const char *dn, Long_t df, Long_t dt);
  void          MiningMonitors(const TVMERawData *rd);

private:
  THttpServer  *fHttpServer; // http server
  TString       fIconPath;   // path to icons
  TDatime       fDatime;     // date/time of monitor

  TList        *fListMSCs;      //! MSC counters
  TMultiGraph  *fMultiPolars;   //! polarizations
  TMultiGraph  *fMultiChambers; //! chambers

  Int_t         fSpills;     // total spills
  ULong_t       fEvents;     // total events

  TGraph       *MakeGraph(const char *gn, const char *gt, Style_t ms, Style_t ls, Color_t c) const;
  void          ResetGraphs(const TList *list) const;
  void          AddGraphPoint(TGraph *g, Double_t value) const;

  ClassDef(THttpMonitor, 1) // HttpMonitor class
};

#endif
