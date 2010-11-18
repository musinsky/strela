// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 18 Nov 2010

#ifndef STRELA_TStrawCham
#define STRELA_TStrawCham

#ifndef ROOT_TList
#include <TList.h>
#endif

#ifndef STRELA_TStrelaBase
#include "TStrelaBase.h"
#endif

class TStrawTracker;
class TStrawLayer;
class TStrawTube;
class TStrawMulti;
class TStrawTrack;

class TStrawCham : public TStrelaBase {

private:
  TList        *fTrackers;  //->list of all trackers
  TList        *fLayers;    //->list of all layeres
  TList        *fMulties;   //->list of all multies(tubes)
  TObjArray    *fTubes;     //->list of all tubes, sorted
  Int_t        *fTubesI;    //  follow up fTubes
  static Int_t  fgTrigNadc; //! unique number from SQL
  static Int_t  fgShiftAdc; //! shift of Adc (for trigger channel)
  static Int_t  fgTracking; //! switching tracking
  static Int_t  fgIter;     //! iteration number

public:
  TStrawCham();
  TStrawCham(const char *name, const char *title);
  virtual      ~TStrawCham();

  TList         *Trackers() const { return fTrackers; }
  TStrawTracker *FindTracker(const char *name) const { return
      (TStrawTracker *)fTrackers->FindObject(name); }
  TList         *Layers() const { return fLayers; }
  TStrawLayer   *FindLayer(const char *name) const { return
      (TStrawLayer *)fLayers->FindObject(name); }
  TList         *Multies() const { return fMulties; }
  TStrawMulti   *FindMulti(const char *name) const { return
      (TStrawMulti *)fMulties->FindObject(name); }
  TObjArray     *Tubes() const { return fTubes; }
  TStrawTube    *GetTube(Int_t it) const { return
      (TStrawTube *)fTubes->UncheckedAt(it); }
  static void   TrigNadc(Int_t tn) { fgTrigNadc = tn; }
  static Int_t  GetTrigNadc() { return fgTrigNadc; }
  static void   ShiftAdc(Int_t sa) { fgShiftAdc = sa; }
  static Int_t  GetShiftAdc() { return fgShiftAdc; }
  static void   Tracking(Int_t mode = 1) { fgTracking = mode; }
  static Int_t  GetTracking() { return fgTracking; }
  static Int_t  GetIter() { return fgIter; }
  Int_t         GetNumOfTracks() const { return GetNumOfContainers(); }
  TStrawTrack  *GetStrawTrack(Int_t it) const { return
      (TStrawTrack *)Container(it); }

  virtual void   Print(Option_t *option = "") const;
  virtual Bool_t ReadSQL(TSQLServer *ser);
  virtual void   AnalyzeBegin();
  virtual void   AnalyzeEntry();
  virtual void   AnalyzeTerminate();

  TStrawTube    *SearchTube(Int_t nadc) const;
  void           DeleteArrays();
  void           SetTubesTMinMax(Int_t del = -1,
                                 Int_t t1 = 0, Int_t t2 = 0) const;
  void           SetTubesCutTime(Option_t *option, Int_t t1, Int_t t2) const;
  static void    IterReset() { fgIter = 0; }
  void           IterNext(Int_t ne = 0);
  void           EfficiencyTubes() const;

  ClassDef(TStrawCham, 1) // StrawCham class
};

#endif
