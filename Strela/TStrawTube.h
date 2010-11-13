// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 01 Nov 2010

#ifndef STRELA_TStrawTube
#define STRELA_TStrawTube

#ifndef ROOT_TEllipse
#include <TEllipse.h>
#endif

#ifndef STRELA_TStrawLayer
#include "TStrawLayer.h"
#endif

class TH1F;
class TH2F;
class TCanvas;

class TStrawMulti;

class TStrawTube : public TEllipse {

public:
  // TStrawTube status bits
  enum {
    kIsDisabled = BIT(14), // don't tracking with this tube
    kNoMultiSum = BIT(15)  // tube don't modifying spline, but read spline
  };
  enum EMarginType { kNoMargin, kMargin, kHalfNeg, kHalfPos };

private:
  Double_t        fCenter;   //  center of straw tube
  Int_t           fT0;       //  zero time
  Int_t           fTMin;     //  minimum time
  Int_t           fTMax;     //  maximum time
  static Double_t fgWireRadius; //! radius of tungsten wire
  Double_t        fDriftVel; //  velocity of drift
  Int_t           fNadc;     //  unique number from SQL
  TStrawLayer    *fLayer;    //! pointer to parent layer
  TStrawMulti    *fMulti;    //! pointer to parent multi
  Int_t           fCutT1;    //  minimum time inside interval
  Int_t           fCutT2;    //  maximum time inside interval
  EMarginType     fMargin;   //  type of tube margin

  static Int_t  fgShowHistograms; //  on/off drawing histograms
  TH1F         *fhTime;    //! raw time (all hits)
  TH1F         *fhCutTime; //! time (if found track) or cutting time
  TH1F         *fhRad1;    //! radius (all hits)
  TH1F         *fhRad2;    //! radius (if found track)
  TH1F         *fhDis1;    //! distance (from real track)
  TH1F         *fhDis2;    //! distance (from track found by tracing method)
  TH1F         *fhEffi;    //! efficiency (dis1/dis2)
  TH2F         *fhTimeRes; //! time vs. residual
  TH2F         *fhDisTime; //! distance vs. time
  TH2F         *fhBzRes;   //! Bz(tube) vs. residual
  void          InitHistograms();
  void          ShowHistoFull(TCanvas *can) const;
  void          ShowHistoCut(TCanvas *can) const;

public:
  TStrawTube();
  TStrawTube(Double_t center);
  virtual      ~TStrawTube();

  void            SetCenter(Double_t center) { fCenter = center; }
  Double_t        GetCenter() const { return fCenter; }
  Int_t           GetT0() const { return fT0; }
  Int_t           GetTMin() const { return fTMin; }
  Int_t           GetTMax() const { return fTMax; }
  static Double_t GetWireRadius() { return fgWireRadius; }
  Double_t        GetDriftVel() const { return fDriftVel; }
  void            SetNadc(Int_t nadc) { fNadc = nadc; }
  Int_t           GetNadc() const { return fNadc; }
  void            SetLayer(TStrawLayer *layer) { fLayer = layer; }
  TStrawLayer    *GetLayer() const { return fLayer; }
  void            SetMulti(TStrawMulti *multi) { fMulti = multi; }
  TStrawMulti    *GetMulti() const { return fMulti; }
  Int_t           GetCutT1() const { return fCutT1; }
  Int_t           GetCutT2() const { return fCutT2; }
  void            SetMargin(EMarginType mtype) { fMargin = mtype; }
  EMarginType     GetMargin() const { return fMargin; }
  Bool_t          IsDisabled() const { return TestBit(kIsDisabled); }
  Bool_t          IsNoMultiSum() const { return TestBit(kNoMultiSum); }

  TH1F         *HisTime() const { return fhTime; }
  TH1F         *HisCutTime() const { return fhCutTime; }
  TH1F         *HisRad1() const { return fhRad1; }
  TH1F         *HisRad2() const { return fhRad2; }
  TH1F         *HisDis1() const { return fhDis1; }
  TH1F         *HisDis2() const { return fhDis2; }
  TH1F         *HisEffi() const { return fhEffi; }
  TH2F         *HisTimeRes() const { return fhTimeRes; }
  TH2F         *HisDisTime() const { return fhDisTime; }
  TH2F         *HisBzRes() const { return fhBzRes; }

  Double_t       GetZ() const { return fLayer->GetZ(); }
  Double_t       GetRange() const { return fLayer->GetRange(); }
  TStrawTracker *GetTracker() const { return fLayer->GetTracker(); }

  virtual Int_t  Compare(const TObject *obj) const;
  virtual void   Draw(Option_t *option = "");
  virtual void   ExecuteEvent(Int_t event, Int_t px, Int_t py);
  virtual char  *GetObjectInfo(Int_t px, Int_t py) const;
  virtual Bool_t IsSortable() const { return kTRUE; }
  virtual void   Print(Option_t *option = "") const;
  virtual const char *GetName() const;
  virtual const char *GetTitle() const;

  void           SetDisabled(Bool_t set = kTRUE); // *TOGGLE* *GETTER=IsDisabled
  void           HighLight(Bool_t flash = kTRUE, Int_t ms = 1000);
  void           SetTime(Int_t t0, Int_t tmax); // *MENU*
  void           SetNoMultiSum(Bool_t set = kTRUE); // *TOGGLE* *GETTER=IsNoMultiSum
  Double_t       T2R(Int_t time) const;
  void           SetShowHistograms(Option_t *option = "") const; // *MENU*
  void           ShowHistograms(Option_t *option = "") const;
  void           SetCutTime(Int_t t1, Int_t t2);
  void           AlignCenter(); // *MENU*
  void           AlignCenterTime(); // *MENU*

  ClassDef(TStrawTube, 1) // StrawTube class
};

#endif
