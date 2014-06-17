// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    17 Jun 2014

#ifndef STRELA_TStrawTracker
#define STRELA_TStrawTracker

#ifndef STRELA_TStrela
#include "TStrela.h"
#endif
#ifndef STRELA_TStrawCham
#include "TStrawCham.h"
#endif
#ifndef STRELA_TStrawTube
#include "TStrawTube.h"
#endif
#ifndef STRELA_TVME
#include "TVME.h"
#endif

class TStrawTracker : public TNamed {

private:
  TList        *fLayers;   //->list of tracker layeres, sorted
  TList        *fTubes;    //->list of tracker tubes
  Int_t         fId;       //  id number of tracker
  Int_t         fNHits;    //  number of hits
  Int_t        *fHit;      //[fNHits] index of hit tube (TStrawCham::fTubes)
  Int_t        *fTime;     //[fNHits] time of tube
  Double_t     *fRadius;   //[fNHits] radius of tube
  Int_t         fMinNHits; //  min. number of hits
  Int_t         fMaxNHits; //  max. number of hits ( < kMaxHits)

  Int_t         fNTracks;       //! number of tracks
  Double_t      fAz;            //! Az of track
  Double_t      fBz;            //! Bz of track
  Double_t      fChi2;          //! Chi2 of track
  Int_t         fTrackNHits;    //! number of hits for track
  Int_t        *fTrackHit;      //![fTrackNHits] index of hits for track

  Int_t         fNPairs;        //  number of pairs
  Int_t        *fPair[2];       //  first & second hit of pair
  Double_t     *fDistPair;      //[fNPairs] distance between pair's tubes
  Double_t      fMinDistPair;   //  min. distance between pair's tubes
  Int_t         fMaxPair;       //  current pair (with max. distance)
  Int_t         fNCheckPairs;   //  number of checked pairs
  Double_t      fAzPair[4];     //  slope of tangent (for current pair)
  Double_t      fBzPair[4];     //  intercept of tangent (for current pair)

  Int_t         fNChecked;      // number of checked hits
  Int_t        *fChecked;       //[fNChecked] index of checked hits
  Int_t        *fAmbiguous;     //[fNChecked] un/ambiguous L/R(U/D)
  Double_t     *fResTangent;    //[fNChecked] residual for tangent
  Int_t         fTrackMinNHits; //  min. number of hits for track
  Double_t      fMaxChi2Tan;    //  max. chi2 for tangent
  Double_t      fMaxDisZTan;    //  max. z-component of drift radius for tangent
  Double_t      fMaxResTan;     //  max. residuals for tangent
  Int_t         fPrecision;     //  method of precision tracking
  static Bool_t fgOnlyOneTrack; //! histo with only one track event

  TH1F         *fhDisZTan;      //! z component of distance of tagent
  TH1F         *fhResTan;       //! residual of tangent
  TH1F         *fhChi2Tan;      //! chi2 of tangent
  TH1F         *fhAz;           //! az
  TH1F         *fhBz;           //! bz
  TH1F         *fhRes;          //! residual
  TH1F         *fhSumR;         //! sum of R
  TH1F         *fhSumD;         //! sum of D
  TH2F         *fhDisRes;       //! distance vs. residual
  TH2F         *fhBzRes;        //! Bz vs. residual
  TH2F         *fhBzAz;         //! Bz vs. Az
  void          InitHistograms();

public:
  TStrawTracker();
  TStrawTracker(Int_t id);
  virtual      ~TStrawTracker();

  TList        *Layers() const { return fLayers; }
  TStrawLayer  *GetLayer(Int_t il) const { return (TStrawLayer *)fLayers->At(il); }
  TStrawLayer  *FindLayer(const char *name) const { return (TStrawLayer *)fLayers->FindObject(name); }
  TList        *Tubes() const { return fTubes; }
  TStrawTube   *GetTube(Int_t it) const { return (TStrawTube *)fTubes->At(it); }
  TStrawTube   *FindTube(const char *name) const { return (TStrawTube *)fTubes->FindObject(name); }
  Int_t         GetId() const { return fId; }
  Int_t         GetNHits() const { return fNHits; }
  void          ResetHits() { fNHits = 0; }
  TStrawTube   *GetTubeHit(Int_t it) const { return gStrela->StrawCham()->GetTube(fHit[it]); }
  Double_t      Z(Int_t ih) const { return GetTubeHit(ih)->GetZ(); }
  Double_t      X(Int_t ih) const { return GetTubeHit(ih)->GetCenter(); }
  Double_t      D(Int_t ih) const { return GetTubeHit(ih)->GetRange(); }
  Double_t      R(Int_t ih) const { return fRadius[ih]; }
  Int_t         T(Int_t ih) const { return fTime[ih]; }
  const char   *ChannelInfo(Int_t ih) const {
    return (ih < fNHits) ? gVME->GetChannelInfo(GetTubeHit(ih)->GetNadc()) : "";
  }

  void        SetMinNHits(Int_t n) { fMinNHits = n; }
  Int_t       GetMinNHits() const { return fMinNHits; }
  Int_t       GetMaxNHits() const { return fMaxNHits; }
  void        SetMinDistPair(Double_t d) { fMinDistPair = d; }
  Double_t    GetMinDistPair() const { return fMinDistPair; }
  void        SetTrackMinNHits(Int_t n) { fTrackMinNHits = n; }
  Int_t       GetTrackMinNHits() const { return fTrackMinNHits; }
  void        SetMaxChi2Tan(Double_t chi2) { fMaxChi2Tan = chi2; }
  Double_t    GetMaxChi2Tan() const { return fMaxChi2Tan; }
  void        SetMaxDisZTan(Double_t dz) { fMaxDisZTan = dz; }
  Double_t    GetMaxDisZTan() const { return fMaxDisZTan; }
  void        SetMaxResTan(Double_t mrt) { fMaxResTan = mrt; }
  Double_t    GetMaxResTan() const { return fMaxResTan; }
  void        SetPrecision(Int_t p) { fPrecision = p; }
  Int_t       GetPrecision() const { return fPrecision; }
  Int_t       GetNTracks() const { return fNTracks; }
  static void   OnlyOneTrack(Bool_t o = kFALSE) { fgOnlyOneTrack = o; }
  static Bool_t IsOnlyOneTrack() { return fgOnlyOneTrack; }

  TH1F       *HisDisZTan() const { return fhDisZTan; }
  TH1F       *HisResTan() const { return fhResTan; }
  TH1F       *HisChi2Tan() const { return fhChi2Tan; }
  TH1F       *HisAz() const { return fhAz; }
  TH1F       *HisBz() const { return fhBz; }
  TH1F       *HisRes() const { return fhRes; }
  TH1F       *HisSumR() const { return fhSumR; }
  TH1F       *HisSumD() const { return fhSumD; }
  TH2F       *HisDisRes() const { return fhDisRes; }
  TH2F       *HisBzRes() const { return fhBzRes; }
  TH2F       *HisBzAz() const { return fhBzAz; }

  virtual void Print(Option_t *option = "") const;

  void         SetMaxNHits(Int_t n);
  void         AllocateLayers();
  void         MarginalTubes(Int_t set = 0, Bool_t onlyhalf = kTRUE) const;
  TStrawTube  *GetTube(Int_t il0, Int_t it0, Bool_t info = kTRUE) const;
  void         ConsecutiveTubes(Int_t itube, Bool_t up, TObjArray *oa) const;
  void         SetMinNHitsAdvanced(Int_t n);
  void         AddHit(Int_t itube, Int_t tdc);
  void         FindTracks();
  TStrawTrack *AddTrack() const;
  void         FindPairs();
  void         LocMaxPair();
  void         TangentsPair(Int_t ia, Int_t ib, Bool_t aonly = kFALSE);
  Bool_t       CheckHits(Int_t it);
  Bool_t       TrackFit(Int_t it);
  void         LFit(Int_t np, const Double_t *x, const Double_t *y,
                    Double_t &a, Double_t &b, Double_t &chi2) const;
  void         DeletePairs();
  void         PrecisionTrackA();
  void         PrecisionTrackB();
  void         FillHistoPerTrack() const;
  void         FillHistoPerEvent() const;
  void         TracingHits() const;
  void         GenerateHits(Double_t a, Double_t b, Double_t sig = 0.01) const;
  void         ShowHistograms(Option_t *option = "") const;
  void         PureTrackHits() const;

  ClassDef(TStrawTracker, 1) // StrawTracker, find tracks
};

#endif
