// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 05 Mar 2008

#ifndef STRELA_TStrawMulti
#define STRELA_TStrawMulti

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif

class TSpline3;
class TH2F;
class TH1F;
class TGraph;
class TFile;

class TStrawLayer;
class TStrawTracker;

class TStrawMulti : public TNamed {

private:
  TList        *fTubes;    //->list of TStrawTube objects
  TSpline3     *fSpline;   //->time to radius spline
  Int_t         fTimeMax;  //  maximum time
  Double_t      fRange;    //  range of straw tube
  TH2F         *fhTimeRes; //! time vs. residual
  TH1F         *fhTime;    //! time(adc)
  TGraph       *fCorrect;  //->autocalibration correction

public:
  TStrawMulti();
  TStrawMulti(const char *name, const char *title = "");
  virtual      ~TStrawMulti();

  TList        *Tubes() const { return fTubes; }
  TSpline3     *GetSpline() const { return fSpline; }
  Int_t         GetTimeMax() const { return fTimeMax; }
  Double_t      GetRange() const { return fRange; }
  TH2F         *HisTimeRes() { SumTimeRes(); return fhTimeRes; }
  TH1F         *HisTime() { SumTime(); return fhTime; }
  TGraph       *GetCorrect() const { return fCorrect; }

  void          Add(const TStrawLayer *layer);
  void          Add(const TStrawTracker *tracker);
  void          AddAll(const TCollection *col);
  void          SumTimeRes();
  void          SumTime();
  void          IterFirst();
  void          IterNext(Int_t ndivide = 50);
  void          ReadWriteSpline(TFile *file);

  ClassDef(TStrawMulti, 1) // StrawMulti, collection of TStrawTube object
};

#endif
