// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    27 May 2014

#ifndef STRELA_TStrela
#define STRELA_TStrela

#include <TChain.h>
#include <TList.h>

class TSQLServer;

class TPommeEvent;
class TStrawEvent;
class TGemEvent;
class TTDCEvent;
class TStrelaBase;
class TWireCham;
class TStrawCham;
class TStrelaDisplay;

class TStrela : public TNamed {

private:
  TString       fSeance;     //  name of seance
  TString       fPath;       //  path to root files
  TChain       *fChain;      //! working tree(chain)

  TList        *fRawEvents;  //->list of raw events
  TPommeEvent  *fPommeEvent; //  pointer to PommeEvent
  TStrawEvent  *fStrawEvent; //  pointer to StrawEvent
  TGemEvent    *fGemEvent;   //  pointer to GemEvent
  TTDCEvent    *fTDCEvent;   //  pointer to TDCEvent

  TList        *fDetectors;  //->list of all detectors
  TWireCham    *fWireCham;   //  pointer to WireCham
  TStrawCham   *fStrawCham;  //  pointer to StrawCham

  TStrelaDisplay *fDisplay;    //! pointer to StrelaDisplay
  TList          *fHistograms; //->list of all histograms

public:
  TStrela();
  TStrela(const char *name, const char *title = "The STRELA Project");
  virtual      ~TStrela();

  void          SetSeance(const char *seance) { fSeance = seance; }
  const char   *GetSeance() const { return fSeance.Data(); }
  void          SetPath(const char *path) { fPath = path; }
  const char   *GetPath() const { return fPath.Data(); }
  TChain       *GetChain() const { return fChain; }
  Int_t         GetEntries() const { return fChain ? (Int_t)fChain->GetEntries() : 0; }
  Int_t         GetEntry(Int_t en) const { return fChain ? fChain->GetEntry(en) : 0; }

  TList        *RawEvents() const { return fRawEvents; }
  TPommeEvent  *PommeEvent() const { return fPommeEvent; }
  TStrawEvent  *StrawEvent() const { return fStrawEvent; }
  TGemEvent    *GemEvent() const { return fGemEvent; }
  TTDCEvent    *TDCEvent() const { return fTDCEvent; }
  TList        *Detectors() const { return fDetectors; }
  Int_t         GetNumOfDetectors() const { return fDetectors->GetSize(); }
  TStrelaBase  *FindDetector(const char *name) const { return (TStrelaBase *)fDetectors->FindObject(name); }
  TWireCham    *WireCham() const { return fWireCham; }
  TStrawCham   *StrawCham() const { return fStrawCham; }

  void            SetDisplay(TStrelaDisplay *dis) { fDisplay = dis; }
  TStrelaDisplay *GetDisplay() const { return fDisplay; }
  TList          *Histograms() const { return fHistograms; }
  TH1            *His(const char *name) const { return (TH1 *)fHistograms->FindObject(name); }

  virtual void   Browse(TBrowser *b);
  virtual Bool_t IsFolder() const { return kTRUE; }

  void          Init() const;
  TSQLServer   *ConnectSQL() const;
  void          SetRun(Int_t run); // *MENU*
  void          ChangeBranchAddress(TChain *chain);
  void          ChangeBranchAddress(TTree *tree) { ChangeBranchAddress((TChain *)tree); }
  void          AnalyzeEntry(Int_t entry) const; // *MENU*
  void          AnalyzeEntries(Int_t ne = 0, Option_t *option = "", Bool_t timer = kFALSE) const;
  void          AddFriend(Option_t *option = "READ", const char *dirname = "DST") const; // *MENU*
  const char   *GetEventInfo() const;
  void          HistoManager(TH1 *his, Option_t *option = "") const;
  void          HistoManager(const char *wildcard = "", Option_t *option = "") const;

  ClassDef(TStrela, 1) // Strela class - top level for all STRELA classes
};

R__EXTERN TStrela *gStrela;

#endif
