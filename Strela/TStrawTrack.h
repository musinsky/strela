// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    24 Oct 2007

#ifndef STRELA_TStrawTrack
#define STRELA_TStrawTrack

#ifndef ROOT_TObject
#include <TObject.h>
#endif

const Int_t kMAXHIT = 50;

class TStrawTrack : public TObject {

private:
  Int_t         fId;      //  Id
  Double32_t    fAz;      //  Az
  Double32_t    fBz;      //  Bz
  Double32_t    fChi2;    //  Chi2

  Int_t         fNHits;        //! NHits
  Int_t         fHit[kMAXHIT]; //! Hit

public:
  TStrawTrack() {;}
  TStrawTrack(Int_t id, Double32_t a, Double32_t b, Double32_t chi2);
  virtual      ~TStrawTrack() {;}

  Int_t         GetId() const { return fId; }
  Double32_t    GetAz() const { return fAz; }
  Double32_t    GetBz() const { return fBz; }
  Double32_t    GetChi2() const { return fChi2; }

  Int_t         GetNHits() const { return fNHits; }
  Int_t         GetHit(Int_t i) const { return (i < fNHits) ? fHit[i] : -1; }

  void          SetHits(Int_t nhits, const Int_t *hit);

  ClassDef(TStrawTrack, 1) // StrawTrack class
};

#endif
