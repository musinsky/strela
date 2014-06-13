// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    13 Jun 2014

#ifndef STRELA_TStrawLayer
#define STRELA_TStrawLayer

#include <TNamed.h>
#include <TList.h>

class TStrawTube;
class TStrawTracker;

class TStrawLayer : public TNamed {

private:
  TList         *fTubes;   //->list of straw tubes
  Double_t       fZ;       //  Z position
  Double_t       fRange;   //  range of straw tube
  Int_t          fNumb;    //  unique number from SQL
  TStrawTracker *fTracker; //! pointer to parent tracker
  Int_t          fConIdx;  //  consecutive index (number) within the frame of tracker

public:
  TStrawLayer();
  TStrawLayer(Int_t numb, const char *name, const char *title = "");
  virtual       ~TStrawLayer();

  TList         *Tubes() const { return fTubes; }
  TStrawTube    *GetTube(Int_t it) const { return (TStrawTube *)fTubes->At(it); }
  TStrawTube    *FindTube(const char *name) const { return (TStrawTube *)fTubes->FindObject(name); }
  void           SetZ(Double_t z) { fZ = z; }
  Double_t       GetZ() const { return fZ; }
  void           SetRange(Double_t r) { fRange = r; }
  Double_t       GetRange() const { return fRange; }
  Int_t          GetNumb() const { return fNumb; }
  void           SetTracker(TStrawTracker *tracker) { fTracker = tracker; }
  TStrawTracker *GetTracker() const { return fTracker; }
  void           SetConIdx(Int_t idx) { fConIdx = idx; }
  Int_t          GetConIdx() const { return fConIdx; }

  virtual Int_t  Compare(const TObject *obj) const;
  virtual Bool_t IsSortable() const { return kTRUE; }

  void           MakeTubes(Int_t n, Double_t del, Int_t dir);
  void           Shift(Double_t shift) const;

  ClassDef(TStrawLayer, 1) // StrawLayer, array of straw tubes
};

#endif
