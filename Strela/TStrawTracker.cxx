// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    01 Jul 2014

#include <TMath.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TRandom.h>

#include "TStrawTracker.h"
#include "TStrawTrack.h"
#include "TVMEEvent.h"

const Int_t kMaxHits    = 50;
const Int_t kMaxPairs   = 200; // sum(kMaxHits - 1)
const Int_t kMaxChecked = 50;  // <= kMaxHits

Bool_t TStrawTracker::fgOnlyOneTrack = kFALSE;

ClassImp(TStrawTracker)

//______________________________________________________________________________
TStrawTracker::TStrawTracker()
{
  //  Info("TStrawTracker", "Default constructor");
  fLayers         = 0;
  fTubes          = 0;
  fId             = -1;
  fNHits          = 0;
  fHit            = 0;
  fTime           = 0;
  fRadius         = 0;
  fMinNHits       = 0;
  fMaxNHits       = 0;
  fNTracks        = 0;
  fAz             = 0;
  fBz             = 0;
  fChi2           = 0;
  fTrackNHits     = 0;
  fTrackHit       = 0;
  fNPairs         = 0;
  fPair[0]        = 0;
  fPair[1]        = 0;
  fDistPair       = 0;
  fMinDistPair    = 0;
  fMaxPair        = 0;
  fNCheckPairs    = 0;
  memset(fAzPair, 0, sizeof(fAzPair));
  memset(fBzPair, 0, sizeof(fBzPair));
  fNChecked       = 0;
  fChecked        = 0;
  fAmbiguous      = 0;
  fResTangent     = 0;
  fTrackMinNHits  = 0;
  fMaxChi2Tan     = 0;
  fMaxDisZTan     = 0;
  fMaxResTan      = 0;
  fPrecision      = 0;
  fhDisZTan       = 0;
  fhResTan        = 0;
  fhChi2Tan       = 0;
  fhAz            = 0;
  fhBz            = 0;
  fhRes           = 0;
  fhSumR          = 0;
  fhSumD          = 0;
  fhDisRes        = 0;
  fhBzRes         = 0;
  fhBzAz          = 0;
}
//______________________________________________________________________________
TStrawTracker::TStrawTracker(Int_t id) : TNamed("", "tracker from straw tubes")
{
  //  Info("TStrawTracker", "Normal constructor");
  fLayers         = 0;
  fTubes          = 0;
  fId             = id;
  SetName(Form("tracker_%d", fId));
  fNHits          = 0;
  fHit            = new Int_t[kMaxHits];
  fTime           = new Int_t[kMaxHits];
  fRadius         = new Double_t[kMaxHits];
  fMinNHits       = 9999; // never
  fMaxNHits       = kMaxHits - 1;

  fNTracks        = 0;
  fAz             = 0;
  fBz             = 0;
  fChi2           = 0;
  fTrackNHits     = 0;
  fTrackHit       = new Int_t[kMaxChecked];

  fNPairs         = 0;
  fPair[0]        = new Int_t[kMaxPairs];
  fPair[1]        = new Int_t[kMaxPairs];
  fDistPair       = new Double_t[kMaxPairs];
  fMinDistPair    = 9999; // never
  fMaxPair        = 0;
  fNCheckPairs    = 0;
  memset(fAzPair, 0, sizeof(fAzPair));
  memset(fBzPair, 0, sizeof(fBzPair));

  fNChecked       = 0;
  fChecked        = new Int_t[kMaxChecked];
  fAmbiguous      = new Int_t[kMaxChecked];
  fResTangent     = new Double_t[kMaxChecked];
  fTrackMinNHits  = 9999; // never
  fMaxChi2Tan     = 9999; // always
  fMaxDisZTan     = 9999; // always
  fMaxResTan      = 9999; // always
  fPrecision      = 0;

  fhDisZTan       = 0;
  fhResTan        = 0;
  fhChi2Tan       = 0;
  fhAz            = 0;
  fhBz            = 0;
  fhRes           = 0;
  fhSumR          = 0;
  fhSumD          = 0;
  fhDisRes        = 0;
  fhBzRes         = 0;
  fhBzAz          = 0;
  InitHistograms();
}
//______________________________________________________________________________
TStrawTracker::~TStrawTracker()
{
  Info("~TStrawTracker", "Destructor");
  delete fLayers; // layers are not deleted
  delete fTubes;  // tubes are not deleted
  delete [] fHit;
  delete [] fTime;
  delete [] fRadius;
  delete [] fTrackHit;
  delete [] fPair[0];
  delete [] fPair[1];
  delete [] fDistPair;
  delete [] fChecked;
  delete [] fAmbiguous;
  delete [] fResTangent;
  delete fhDisZTan; fhDisZTan = 0;
  delete fhResTan;  fhResTan = 0;
  delete fhChi2Tan; fhChi2Tan = 0;
  delete fhAz;      fhAz = 0;
  delete fhBz;      fhBz = 0;
  delete fhRes;     fhRes = 0;
  delete fhSumR;    fhSumR = 0;
  delete fhSumD;    fhSumD = 0;
  delete fhDisRes;  fhDisRes = 0;
  delete fhBzRes;   fhBzRes = 0;
  delete fhBzAz;    fhBzAz = 0;
}
//______________________________________________________________________________
void TStrawTracker::Print(Option_t *option) const
{
  TStrawCham *cham = gStrela->StrawCham();
  TStrawTube *tube;
  TStrawTrack *track;

  for (Int_t ih = 0; ih < fNHits; ih++) {
    tube = GetTubeHit(ih);
    printf("%2d) %s, %5d ~ %5.3f", ih, gVME->GetChannelInfo(tube->GetNadc()), T(ih), R(ih));
    if (!strcmp(option, "all")) {
      for (Int_t it = 0; it < cham->GetNumOfTracks(); it++) {
        track = cham->GetStrawTrack(it);
        if (track->GetId() != fId) continue;
        for (Int_t ith = 0; ith < track->GetNHits(); ith++)
          if (ih == track->GetHit(ith)) printf(", track_%d", it);
      }
    }
    printf("\n");
  }

  if (strcmp(option, "all")) return;
  for (Int_t it = 0; it < cham->GetNumOfTracks(); it++) {
    track = cham->GetStrawTrack(it);
    if (track->GetId() != fId) continue;
    Printf("%d) nhits = %2d, az = %7.4f, bz = %6.2f, chi2 = %f", it,
           track->GetNHits(), track->GetAz(), track->GetBz(), track->GetChi2());
  }
}
//______________________________________________________________________________
void TStrawTracker::InitHistograms()
{
  fhAz = new TH1F(Form("%s_az", GetName()), "slope; rad", 1000, -0.12, 0.12);
  fhAz->SetMinimum(1);
  gStrela->HistoManager(fhAz, "add");
  fhBz = new TH1F(Form("%s_bz", GetName()), "intercept; cm", 1000, -13, 13);
  fhBz->SetMinimum(1);
  gStrela->HistoManager(fhBz, "add");
  fhRes = new TH1F(Form("%s_res", GetName()), "residual; cm", 1000, -0.15, 0.15);
  fhRes->SetMinimum(1);
  gStrela->HistoManager(fhRes, "add");
  fhResTan = new TH1F(Form("%s_restan", GetName()), "residual (candidate); cm", 1000, -0.15, 0.15);
  fhResTan->SetMinimum(1);
  fhResTan->SetLineColor(kGray + 1);
  gStrela->HistoManager(fhResTan, "add");
  fhSumR = new TH1F(Form("%s_sumR", GetName()), "sum of R; cm", 1000, 1.8, 2.4);
  fhSumR->SetMinimum(1);
  fhSumR->SetLineColor(kGray + 1);
  gStrela->HistoManager(fhSumR, "add");
  fhSumD = new TH1F(Form("%s_sumD", GetName()), "sum of D; cm", 1000, 1.8, 2.4);
  fhSumD->SetMinimum(1);
  gStrela->HistoManager(fhSumD, "add");
  fhDisZTan = new TH1F(Form("%s_dzt", GetName()), "z - component of distance from wire (candidate); cm",
                       1000, -0.5, 0.5);
  fhDisZTan->SetMinimum(0);
  gStrela->HistoManager(fhDisZTan, "add");
  fhChi2Tan = new TH1F(Form("%s_chi2t", GetName()), "#chi^{2}/ndf (candidate)", 1000, 0, 0.02);
  fhChi2Tan->SetMinimum(0);
  gStrela->HistoManager(fhChi2Tan, "add");

  fhDisRes = new TH2F(Form("%s_dis_res", GetName()), "distance from wire : residual; dist., cm; residual, cm",
                      100, -2.2, 2.2, 100, -0.15, 0.15);
  fhDisRes->GetYaxis()->CenterTitle();
  gStrela->HistoManager(fhDisRes, "add");
  fhBzRes = new TH2F(Form("%s_bz_res", GetName()), "intercept : residual; intercept, cm; residual, cm",
                     100, -13, 13, 100, -0.15, 0.15);
  fhBzRes->GetYaxis()->CenterTitle();
  gStrela->HistoManager(fhBzRes, "add");
  fhBzAz = new TH2F(Form("%s_bz_az", GetName()), "intercept : slope; intercept, cm; slope, rad",
                    130, -13, 13, 120, -0.12, 0.12);
  fhBzAz->GetYaxis()->CenterTitle();
  gStrela->HistoManager(fhBzAz, "add");
}
//______________________________________________________________________________
void TStrawTracker::SetMaxNHits(Int_t n)
{
  fMaxNHits = n;
  if (fMaxNHits >= kMaxHits) {
    Warning("SetMaxNHits", "max. hits must be < %d", kMaxHits);
    fMaxNHits = kMaxHits - 1;
  }
}
//______________________________________________________________________________
void TStrawTracker::AllocateLayers()
{
  // called (allocated) only once
  if (!fLayers || !fTubes) {
    fLayers = new TList();
    fTubes  = new TList();
  }
  else {
    Warning("AllocateLayers", "%s re-allocated layers list", GetName());
    fLayers->Clear();
    fTubes->Clear();
  }

  TIter next(gStrela->StrawCham()->Layers());
  TStrawLayer *l;
  while ((l = (TStrawLayer *)next()))
    if (l->GetTracker() == this) fLayers->Add(l);
  fLayers->Sort(); // sort by increasing Z (tubes in each layer are ordered by coordinate)

  TIter layers(fLayers);
  TStrawLayer *layer;
  while ((layer = (TStrawLayer *)layers())) {
    layer->SetConIdx(fLayers->IndexOf(layer));
    TIter tubes(layer->Tubes());
    TStrawTube *tube;
    while ((tube = (TStrawTube *)tubes())) {
      fTubes->Add(tube); // tubes are fully ordered by geometry of tracker (not by increasing Nadc)
    }
  }

  // find layers with half tubes
  Int_t nt, nt0 = GetLayer(0)->Tubes()->GetSize();
  Int_t maxtubes = nt0, maxdelta = 0;
  for (Int_t il = 1; il < fLayers->GetSize(); il++) {
    nt = GetLayer(il)->Tubes()->GetSize();
    maxtubes = TMath::Max(nt, maxtubes);
    maxdelta = TMath::Max(TMath::Abs(nt0 - nt), maxdelta);
  }
  if (maxdelta != 1) {
    Info("AllocateLayers", "cannot determine half-layers in %s", GetName());
    maxtubes = -1;
  }

  TStrawTube *first, *last;
  for (Int_t il = 0; il < fLayers->GetSize(); il++) {
    first = (TStrawTube *)GetLayer(il)->Tubes()->First();
    last  = (TStrawTube *)GetLayer(il)->Tubes()->Last();
    if (GetLayer(il)->Tubes()->GetSize() == maxtubes) {
      first->SetMargin(TStrawTube::kHalfPos);
      last->SetMargin(TStrawTube::kHalfNeg);
    }
    else {
      first->SetMargin(TStrawTube::kMargin);
      last->SetMargin(TStrawTube::kMargin);
    }
  }
}
//______________________________________________________________________________
void TStrawTracker::MarginalTubes(Int_t set, Bool_t onlyhalf) const
{
  TIter tubes(fTubes);
  TStrawTube *tube;
  while ((tube = (TStrawTube *)tubes())) {
    if (tube->GetMargin() == TStrawTube::kNoMargin) continue;
    if (onlyhalf && (tube->GetMargin() == TStrawTube::kMargin)) continue;
    switch (set) {
      case 0: // to default
        tube->SetDisabled(kFALSE);
        tube->SetNoMultiSum(kFALSE);
        break;
      case 1:
        tube->SetNoMultiSum(kTRUE);
        break;
      case -1:
        tube->SetNoMultiSum(kFALSE);
        break;
      case 2:
        tube->SetDisabled(kTRUE);
        break;
      case -2:
        tube->SetDisabled(kFALSE);
        break;
      default:
        Warning("MarginalTubes", "%s without changing", tube->GetName());
        break;
    }
  }
}
//______________________________________________________________________________
TStrawTube *TStrawTracker::GetTube(Int_t il0, Int_t it0, Bool_t info) const
{
  TStrawLayer *layer = GetLayer(il0);
  if (!layer || (il0 < 0)) {
    Printf("%s: %d-th layer does not exist", GetName(), il0);
    return 0;
  }
  TStrawTube *tube = layer->GetTube(it0);
  if (!tube || (it0 < 0)) {
    Printf("%s: %d-th tube of %dth layer does not exist", GetName(), it0, il0);
    return 0;
  }

  if (info) {
    Printf("%s", tube->GetTitle());
    tube->HighLight();
  }

  return tube;
}
//______________________________________________________________________________
void TStrawTracker::ConsecutiveTubes(Int_t itube, Bool_t up, TObjArray *oa) const
{
  if (!oa) return;
  oa->Clear();
  if (fLayers->GetSize() != 4) {
    Info("ConsecutiveTubes", "only for 4-layers tracker");
    return;
  }
  if ((GetTube(0, 0, kFALSE)->GetMargin() != TStrawTube::kHalfPos) ||
      (GetTube(1, 0, kFALSE)->GetMargin() != TStrawTube::kMargin)  ||
      (GetTube(2, 0, kFALSE)->GetMargin() != TStrawTube::kHalfPos) ||
      (GetTube(3, 0, kFALSE)->GetMargin() != TStrawTube::kMargin)) {
    Info("ConsecutiveTubes", "only for specific pattern");
    return;
  }

  for (Int_t il = 0; il < fLayers->GetSize(); il++) {
    if ((il%2) && !up) oa->AddAt(GetTube(il, itube - 1, kFALSE), il);
    else               oa->AddAt(GetTube(il, itube,     kFALSE), il);
  }
}
//______________________________________________________________________________
void TStrawTracker::SetMinNHitsAdvanced(Int_t n)
{
  if ((fLayers->GetSize() != 4) || (n < 3) || (n > 4)) {
    Info("SetMinNHitsAdvanced", "you must set custom parameters");
    fMinNHits = fTrackMinNHits = fMinDistPair = 9999; // never
    return;
  }

  fMinNHits = fTrackMinNHits = n;
  // fLayers is sorted
  fMinDistPair = TMath::Abs(((TStrawLayer *)fLayers->At(0))->GetZ() -
                            ((TStrawLayer *)fLayers->At(n - 1))->GetZ()) - 0.1;
}
//______________________________________________________________________________
void TStrawTracker::AddHit(Int_t itube, Int_t tdc)
{
  if (fNHits >= kMaxHits) {
    if (gDebug > 0) {
      Info("AddHit", "to many hits in %s, limit is %d", GetName(), kMaxHits);
      Printf("%s", gStrela->GetEventInfo());
    }
    return;
  }

  fHit[fNHits]    = itube;
  fTime[fNHits]   = tdc;
  fRadius[fNHits] = GetTubeHit(fNHits)->T2R(tdc);
  fNHits++;
}
//______________________________________________________________________________
void TStrawTracker::FindTracks()
{
  fNTracks = 0;
  // number of hits must be exact in <fMinNHits, fMaxNHits>
  if ((fNHits < fMinNHits) || (fNHits > fMaxNHits)) return;

  FindPairs();                             // find all posible pairs from hits
  if (fNPairs == 0) return;                // exist pair ?
  do {                                     // checking all pairs
    LocMaxPair();                          // pair with max. distance
    Bool_t findTrack = kFALSE;             // reset
    for (Int_t t = 0; t < 4; t++)          // loop over all 4 tangents
      if (CheckHits(t)) findTrack = kTRUE; // finding the best track
    if (findTrack) {                       // exist track ?
      DeletePairs();                       // delete pairs with assigned hits
      if (fPrecision == 3) {               // both precision tracking method
        PrecisionTrackB();
        PrecisionTrackA();
      }
      else if (fPrecision == 1)            // only method A
        PrecisionTrackA();
      else if (fPrecision == 2)            // only method B (no sense, same as default)
        PrecisionTrackB();
      AddTrack();                          // add track
      if (!fgOnlyOneTrack)
        FillHistoPerTrack();               // filling histograms (each track)
      fNTracks++;
    }
  } while (fNCheckPairs < fNPairs);        // next pair

  if (fgOnlyOneTrack && (fNTracks == 1))
    FillHistoPerTrack();                   // only one track event
  FillHistoPerEvent();                     // filling histograms (each event)
}
//______________________________________________________________________________
TStrawTrack *TStrawTracker::AddTrack() const
{
  TClonesArray &tracks = *(gStrela->StrawCham()->Containers());
  Int_t it = tracks.GetLast() + 1;
  TStrawTrack *track = new(tracks[it]) TStrawTrack(fId, fAz, fBz, fChi2);
  track->SetHits(fTrackNHits, fTrackHit); // without streaming
  return track;
}
//______________________________________________________________________________
void TStrawTracker::FindPairs()
{
  fNPairs      = 0;
  fNCheckPairs = 0;
  Double_t distance;

  for (Int_t i = 0; i < fNHits; i++) {
    for (Int_t j = i+1; j < fNHits; j++) {
      distance = TMath::Abs(Z(j) - Z(i));
      if (distance > fMinDistPair) {
        if (fNPairs >= kMaxPairs) {
          fNPairs = 0;
          if (gDebug > 0) {
            Error("FindPairs", "to many pairs, limit is %d", kMaxPairs);
            Printf("%s", gStrela->GetEventInfo());
          }
          return;
        }

        fPair[0][fNPairs]  = i;
        fPair[1][fNPairs]  = j;
        fDistPair[fNPairs] = distance;
        fNPairs++;
      }
    }
  }

#ifdef DEBUG
  Printf("\n%s, %s", GetName(), gStrela->GetEventInfo());
  Printf("FindPairs => number of pairs: %d", fNPairs);
  for (Int_t i = 0; i < fNPairs; i++) {
    printf("%2d) ", i);
    Printf("%s & %s", ChannelInfo(fPair[0][i]), ChannelInfo(fPair[1][i]));
  }
#endif /* DEBUG */
}
//______________________________________________________________________________
void TStrawTracker::LocMaxPair()
{
  Double_t dmax = 0.0;

  for (Int_t i = 0; i < fNPairs; i++) {
    if (fDistPair[i] > dmax) {
      dmax     = fDistPair[i];
      fMaxPair = i;
    }
  }

  fChi2       = fMaxChi2Tan;
  fTrackNHits = fTrackMinNHits;

  fDistPair[fMaxPair] = -1.0; // must be negative, see DeletePairs()
  TangentsPair(fPair[0][fMaxPair], fPair[1][fMaxPair]);
  fNCheckPairs++;

#ifdef DEBUG
  printf(" LocMaxPair => pair with max. distance\n %2d) %s & %s\n", fMaxPair,
         ChannelInfo(fPair[0][fMaxPair]), ChannelInfo(fPair[1][fMaxPair]));
#endif /* DEBUG */
}
//______________________________________________________________________________
void TStrawTracker::TangentsPair(Int_t ia, Int_t ib, Bool_t aonly)
{
  Int_t i0 = ia, i1 = ib; // make sure firstZ(x) < secondZ(x)
  if (Z(ia) > Z(ib)) { i0 = ib; i1 = ia; }
  Double_t x[2] = { Z(i0), Z(i1) };
  Double_t y[2] = { X(i0), X(i1) };
  Double_t r[2] = { R(i0), R(i1) };

  Double_t xd     = x[0] - x[1];
  Double_t yd     = y[0] - y[1];
  Double_t rm     = r[0] - r[1];
  Double_t rp     = r[0] + r[1];
  Double_t xd2    = xd*xd;
  Double_t yd2    = yd*yd;
  Double_t xdyd   = xd*yd;
  Double_t sxdyd2 = xd2 + yd2;
  Double_t rm2    = rm*rm;
  Double_t rp2    = rp*rp;

  Double_t a11    = xdyd + rm*TMath::Sqrt(sxdyd2 - rm2);
  Double_t a12    = xdyd - rm*TMath::Sqrt(sxdyd2 - rm2);
  Double_t b1     = rm2 - xd2;
  Double_t a21    = xdyd + rp*TMath::Sqrt(sxdyd2 - rp2);
  Double_t a22    = xdyd - rp*TMath::Sqrt(sxdyd2 - rp2);
  Double_t b2     = rp2 - xd2;

  fAzPair[0] = -a11/b1;
  fAzPair[1] = -a12/b1;
  fAzPair[2] = -a21/b2;
  fAzPair[3] = -a22/b2;

  if (sxdyd2 < rp2) fAzPair[2] = fAzPair[3] = -xd/yd; // intersect two circles
  if (aonly) return;

  Int_t sign[4] = {-1, +1, +1, -1};
  for (Int_t it = 0; it < 4; it++)
    fBzPair[it] = -fAzPair[it]*x[1] + y[1];

  if (xd == 0) { // neighbors hits
    Info("TangentsPair", "%s, (xd == 0) of %s and %s", GetName(),
         GetTubeHit(i0)->GetName(), GetTubeHit(i1)->GetName());
    Printf("%s", gStrela->GetEventInfo());

    if (y[0] < y[1]) {
      sign[2] = -sign[2];
      if (r[0] < r[1]) sign[1] = -sign[1];
      else             sign[0] = -sign[0];
    }
    else {
      sign[3] = -sign[3];
      if (r[0] < r[1]) sign[0] = -sign[0];
      else             sign[1] = -sign[1];
    }
  }
  else if (xd2 < rp2) { // neighbors hits
    Info("TangentsPair", "%s, (xd2 < rp2) of %s and %s", GetName(),
         GetTubeHit(i0)->GetName(), GetTubeHit(i1)->GetName());
    Printf("%s", gStrela->GetEventInfo());

    if (y[0]  < y[1]) sign[2] = -sign[2];
    else              sign[3] = -sign[3];
  }

  for (Int_t it = 0; it < 4; it++)
    fBzPair[it] += sign[it]*r[1]*TMath::Sqrt(1.0 + fAzPair[it]*fAzPair[it]);

#ifdef DEBUG
  // see TStrelaDisplay::DisplayEvent()
  for (Int_t i = 0; i < 4; i++) {
    fAz = fAzPair[i];
    fBz = fBzPair[i];
    fTrackNHits = 0; // marking tangent
    AddTrack();
  }
  fTrackNHits = fTrackMinNHits;
#endif /* DEBUG */
}
//______________________________________________________________________________
Bool_t TStrawTracker::CheckHits(Int_t it)
{
  fNChecked = 0;
  Bool_t tangentHit, multiHit;
  Double_t d, dz, dabs, res;
  Double_t d0 = TMath::Sqrt(1.0 + fAzPair[it]*fAzPair[it]);

  for (Int_t i = 0; i < fNHits; i++) {
    tangentHit = ((fPair[0][fMaxPair] == i) || (fPair[1][fMaxPair] == i)) ? kTRUE : kFALSE;

    d  = (fAzPair[it]*Z(i) - X(i) + fBzPair[it])/d0;
    dz = d*fAzPair[it]/d0; // dz for radius = R(i)*fAzPair[it]/d0;
    fhDisZTan->Fill(dz);
    if (TMath::Abs(dz) > fMaxDisZTan) {
      if (tangentHit) return kFALSE; // skip tangent(track)
      continue; // skip hit
    }

    dabs = TMath::Abs(d);
    if (dabs > (D(i)*1.10)) continue; // tangent must intersect tube (plus/minus)
    // FIXME
    res  = R(i) - dabs;
    if (!tangentHit) fhResTan->Fill(res);
    res  = TMath::Abs(res);
    if (res > fMaxResTan) continue;

    multiHit = kFALSE;
    for (Int_t ii = 0; ii < fNChecked; ii++) { // only one hit per tube
      if (GetTubeHit(fChecked[ii]) == GetTubeHit(i)) {
        multiHit = kTRUE;
        if (fResTangent[ii] > res) {
          fChecked[ii]              = i;
          if (d > 0) fAmbiguous[ii] = +1;
          else       fAmbiguous[ii] = -1;
          fResTangent[ii]           = res;
        }
        break;
      }
    }
    if (multiHit) continue;

    if (fNChecked >= kMaxChecked) {
      if (gDebug > 0) {
        Error("CheckHits", "to many checked hits, limit is %d", kMaxChecked);
        Printf("%s", gStrela->GetEventInfo());
      }
      return kFALSE;
    }

    fChecked[fNChecked]              = i;
    if (d > 0) fAmbiguous[fNChecked] = +1;
    else       fAmbiguous[fNChecked] = -1;
    fResTangent[fNChecked]           = res;
    fNChecked++;
  }

  if (fNChecked >= fTrackNHits) return TrackFit(it);
  return kFALSE;
}
//______________________________________________________________________________
Bool_t TStrawTracker::TrackFit(Int_t it)
{
  Double_t x[kMaxChecked], z[kMaxChecked];
  Double_t rs = TMath::Sqrt(1.0 + fAzPair[it]*fAzPair[it]);
  Int_t itube, sorted[kMaxChecked];
  TMath::Sort(fNChecked, fResTangent, sorted, kFALSE);

  for (Int_t i = 0; i < fNChecked; i++) {
    itube = fChecked[sorted[i]];
    x[i]  = X(itube);
    x[i] += fAmbiguous[sorted[i]]*R(itube)*1.0/rs;
    z[i]  = Z(itube);
    z[i] -= fAmbiguous[sorted[i]]*R(itube)*fAzPair[it]/rs;
  }

  // nhits on current tangent(track) is >= nhits from previous tangent(track)
  //
  // 1) number of hits is bigger then previous (but always chi2 < maxChi2)
  // 2) chi2 is smaller then previous (but always nhits >= trackMinNHits)

  Double_t alfa, beta, chi2;
  for (Int_t nhits = fNChecked; nhits >= fTrackNHits; nhits--) {
    LFit(nhits, z, x, alfa, beta, chi2);
    fhChi2Tan->Fill(chi2);
    if ((nhits == fTrackNHits) && (chi2 > fChi2)) continue;
    if (chi2 > fMaxChi2Tan) continue;
    fAz         = alfa;
    fBz         = beta;
    fChi2       = chi2;
    fTrackNHits = nhits;
    for (Int_t ih = 0; ih < fTrackNHits; ih++)
      fTrackHit[ih] = fChecked[sorted[ih]];
    return kTRUE;
  }
  return kFALSE;
}
//______________________________________________________________________________
void TStrawTracker::LFit(Int_t np, const Double_t *x, const Double_t *y,
                         Double_t &a, Double_t &b, Double_t &chi2) const
{
  Double_t sumx = 0.0, sumy = 0.0, sumxy = 0.0, sumxx = 0.0, sumyy = 0.0;
  for (Int_t i = 0; i < np; i++) {
    sumx += x[i];
    sumy += y[i];
  }

  Double_t xmed = sumx/np, ymed = sumy/np;
  Double_t scartx, scarty;

  for (Int_t j = 0; j < np; j++) {
    scartx = x[j] - xmed;
    scarty = y[j] - ymed;
    sumxx += scartx*scartx;
    sumyy += scarty*scarty;
    sumxy += scartx*scarty;
  }

  if (TMath::Abs(sumxx) < 1.E-12) {
    Warning("LFit", "something wrong with %s geometry", GetName());
    a = 0.0; b = 0.0; chi2 = 9999;
    return;
  }

  a    = sumxy/sumxx;
  b    = ymed - a*xmed;
  chi2 = (sumyy - a*sumxy)/(np - 2.0);
}
//______________________________________________________________________________
void TStrawTracker::DeletePairs()
{
  for (Int_t ih = 0; ih < fTrackNHits; ih++) {
    for (Int_t ip = 0; ip < fNPairs; ip++) {
      if (fDistPair[ip] < 0.0) continue; // skip previous remove pairs
      if ((fPair[0][ip] == fTrackHit[ih]) || (fPair[1][ip] == fTrackHit[ih])) {
        fDistPair[ip] = -1.0;
        fNCheckPairs++;
      }
    }
  }

#ifdef DEBUG
  Printf(" DeletePairs => number of checked pairs: %d", fNCheckPairs);
#endif /* DEBUG */
}
//______________________________________________________________________________
void TStrawTracker::PrecisionTrackA()
{
  // find tangents between two neighbors hits

  if (2*(fTrackNHits - 1) >= kMaxChecked) {
    Error("PrecisionTrack", "to many track hits, limit is %d", kMaxChecked);
    Printf("%s", gStrela->GetEventInfo());
    return;
  }

  Double_t z[kMaxChecked], x[kMaxChecked];
  Int_t i1, i2, count = 0, amb[kMaxChecked], sort[kMaxChecked];
  Double_t az = 0.0, d, d0 = TMath::Sqrt(1.0 + fAz*fAz);

  for (Int_t i = 0; i < fTrackNHits; i++) {
    i1 = fTrackHit[i];
    d  = (fAz*Z(i1) - X(i1) + fBz)/d0;
    if (d > 0) amb[i] = +1;
    else       amb[i] = -1;
    z[i] = Z(i1) - amb[i]*R(i1)*fAz/d0;
  }
  TMath::Sort(fTrackNHits, z, sort, kFALSE);

  for (Int_t i = 0; i < (fTrackNHits - 1); i++) {
    i1 = fTrackHit[sort[i]];
    i2 = fTrackHit[sort[i+1]];
    TangentsPair(i1, i2, kTRUE);

    if      ((amb[sort[i]] > 0) && (amb[sort[i+1]] > 0)) az = fAzPair[1];
    else if ((amb[sort[i]] > 0) && (amb[sort[i+1]] < 0)) az = fAzPair[3];
    else if ((amb[sort[i]] < 0) && (amb[sort[i+1]] > 0)) az = fAzPair[2];
    else if ((amb[sort[i]] < 0) && (amb[sort[i+1]] < 0)) az = fAzPair[0];
    // work correct also for tubes with same Z

    d0 = TMath::Sqrt(1.0 + az*az);
    x[count] = X(i1) + amb[sort[i]]*R(i1)*1.0/d0;
    z[count] = Z(i1) - amb[sort[i]]*R(i1)*az/d0;
    count++;
    x[count] = X(i2) + amb[sort[i+1]]*R(i2)*1.0/d0;
    z[count] = Z(i2) - amb[sort[i+1]]*R(i2)*az/d0;
    count++;
  }

  LFit(count, z, x, fAz, fBz, fChi2);
}
//______________________________________________________________________________
void TStrawTracker::PrecisionTrackB()
{
  // iterative method
  // this method is practically identical to the default method

  Double_t z[kMaxChecked], x[kMaxChecked];
  Int_t ih, amb;
  Double_t d, d0, az;

  for (Int_t iter = 0; iter < 5; iter++) { // max number of iteration
    d0 = TMath::Sqrt(1.0 + fAz*fAz);

    for (Int_t i = 0; i < fTrackNHits; i++) {
      ih = fTrackHit[i];
      d  = (fAz*Z(ih) - X(ih) + fBz)/d0;
      if (d > 0) amb = +1;
      else       amb = -1;
      z[i] = Z(ih) - amb*R(ih)*fAz/d0;
      x[i] = X(ih) + amb*R(ih)*1.0/d0;
    }

    az = fAz;
    LFit(fTrackNHits, z, x, fAz, fBz, fChi2);
    if (TMath::Abs(fAz - az) < 1.E-10) break; // ~ 2-3 iteration
  }
}
//______________________________________________________________________________
void TStrawTracker::FillHistoPerTrack() const
{
  fhAz->Fill(fAz);
  fhBz->Fill(fBz);
  fhBzAz->Fill(fBz, fAz); // fAz, fBz from projection ?!

  Int_t ihit;
  Double_t d, res, d0 = TMath::Sqrt(1.0 + fAz*fAz), sumr = 0.0, sumd = 0.0;
  TStrawTube *tube;

  for (Int_t i = 0; i < fTrackNHits; i++) {
    ihit = fTrackHit[i];
    tube = GetTubeHit(ihit);
    d    = (fAz*Z(ihit) - X(ihit) + fBz)/d0;
    res  = R(ihit) - TMath::Abs(d);
    sumr += R(ihit);
    sumd += TMath::Abs(d);

    fhRes->Fill(res);
    fhDisRes->Fill(d, res);
    fhBzRes->Fill(fBz, res);

    tube->HisTime2()->Fill(tube->TExT0(T(ihit)));
    tube->HisRad2()->Fill(R(ihit));
    tube->HisDis1()->Fill(d);
    tube->HisTimeRes()->Fill(T(ihit), res);
    tube->HisDisTime()->Fill(d, T(ihit));
    tube->HisBzRes()->Fill(fBz - tube->GetCenter(), res);
    tube->HisBzAz()->Fill(fBz - tube->GetCenter(), fAz);
  }

  if ((fLayers->GetSize() == 4) && (fTrackNHits == 4)) { // only 4layers tracker
    fhSumR->Fill(sumr/2.0);
    fhSumD->Fill(sumd/2.0);
  }
  TracingHits();
}
//______________________________________________________________________________
void TStrawTracker::FillHistoPerEvent() const
{
  ;
}
//______________________________________________________________________________
void TStrawTracker::TracingHits() const
{
  TStrawLayer *layer;
  TStrawTube *tube;
  Double_t d, d0 = TMath::Sqrt(1.0 + fAz*fAz);

  TIter nextl(fLayers);
  while ((layer = (TStrawLayer *)nextl())) {
    Bool_t found = kFALSE;
    TIter nextt(layer->Tubes());
    while ((tube = (TStrawTube *)nextt())) {
      d = (fAz*tube->GetZ() - tube->GetCenter() + fBz)/d0;
      if (TMath::Abs(d) <= tube->GetRange()) {
        found = kTRUE;
        tube->HisDis2()->Fill(d);
      }
      else if (found) break; // hits in one layer go consecutive
    }
  }
}
//______________________________________________________________________________
void TStrawTracker::GenerateHits(Double_t a, Double_t b, Double_t sig) const
{
  // need for Monte Carlo

  TStrawLayer *layer;
  TStrawTube *tube;
  Double_t d, d0 = TMath::Sqrt(1.0 + a*a);

  TIter nextl(fLayers);
  while ((layer = (TStrawLayer *)nextl())) {
    Bool_t found = kFALSE;
    TIter nextt(layer->Tubes());
    while ((tube = (TStrawTube *)nextt())) {
      d  = (a*tube->GetZ() - tube->GetCenter() + b)/d0;
      if (TMath::Abs(d) <= tube->GetRange()) {
        found = kTRUE;
        d += gRandom->Gaus(0, sig);
        gStrela->VMEEvent()->AddTDCHit(tube->GetNadc(), tube->TExT0(tube->R2T(d)));
      }
      else if (found) break; // hits in one layer go consecutive
    }
  }
}
//______________________________________________________________________________
void TStrawTracker::ShowHistograms(Option_t * /*option*/) const
{
  TVirtualPad *save = gPad;
  TCanvas *c = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("c_tracker");
  if (!c) {
    // draw tracker canvas left of tube canvas
    Int_t wwTube, ww = 620;
    TCanvas *cTube = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("c_tube");
    if (cTube) wwTube = cTube->GetWindowTopX() - 12;
    else       wwTube = ww;

    c = new TCanvas("c_tracker", "", wwTube - ww, 0, ww, ww*0.80);
    c->Divide(2, 4, 0.001, 0.001);
    c->GetPad(3)->SetGrid();
    c->GetPad(4)->SetGrid();
  }
  else c->Clear("D"); // subpads are cleared but not deleted

  c->cd(1);
  fhAz->Draw();
  c->cd(2);
  fhBz->Draw();
  c->cd(3);
  fhDisRes->Draw("COL");
  c->cd(4);
  fhBzRes->Draw("COL");
  c->cd(5);
  fhSumD->Draw();
  fhSumR->Draw("same");
  c->cd(6);
  fhRes->Draw();
  fhResTan->Draw("same");
  c->cd(7);
  fhChi2Tan->Draw();
  c->cd(8);
  //  fhDisZTan->Draw();
  fhBzAz->Draw("COL");

  c->SetTitle(GetName());
  c->Update();
  if (save) save->cd();
}
//______________________________________________________________________________
void TStrawTracker::PureTrackHits() const
{
  // better use macros/tracker_TDC.C

  TList straight1, straight2;
  TStrawTube *tube;
  Double_t x1 = 9999, x2 = -9999;
  Int_t count = 0, pureHit[kMaxHits];

  for (Int_t i = 0; i < fNHits; i++) {
    tube = GetTubeHit(i);
    if (straight1.FindObject(tube)) return; // only one hit per wire
    if (straight1.IsEmpty()) x1 = X(i);
    else if (x1 != X(i)) continue; // hits with same X
    pureHit[count++] = i;          // first 2-hits
    straight1.Add(tube);
  }
  if (straight1.GetSize() < Int_t(fLayers->GetSize()/2.0)) return; // odd-even

  for (Int_t i = 0; i < fNHits; i++) {
    tube = GetTubeHit(i);
    if (straight2.FindObject(tube)) return;
    if (straight1.FindObject(tube)) continue;
    if (TMath::Abs(x1 - X(i)) > (D(i) + 10e-06)) continue;
    if (straight2.IsEmpty()) x2 = X(i);
    else if (x2 != X(i)) continue;
    pureHit[count++] = i;          // second 2-hits
    straight2.Add(tube);
  }
  if ((straight1.GetSize() + straight2.GetSize()) != fLayers->GetSize()) return;

  for (Int_t ih = 0; ih < count; ih++) {
    tube = GetTubeHit(pureHit[ih]);
    tube->HisTime2()->Fill(tube->TExT0(T(pureHit[ih])));
  }
}
