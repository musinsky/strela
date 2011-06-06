// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    26 May 2011

#ifndef STRELA_TPommeEvent
#define STRELA_TPommeEvent

#ifndef STRELA_TVME
#include "TVME.h"
#endif
#ifndef STRELA_TStrawCham
#include "TStrawCham.h"
#endif

class TChWire : public TObject {

private:
  UShort_t      fWire;  //  Wire
  UShort_t      fWidth; //  Width

public:
  TChWire() {;}
  virtual      ~TChWire() {;}

  Int_t         GetWire() const { return (Int_t)fWire; }
  Int_t         GetWidth() const { return (Int_t)fWidth; }

  ClassDef(TChWire, 2) // ChWire class (not original)
};

//______________________________________________________________________________

class TAdcHit : public TObject {

private:
  UShort_t      fNadc; //  Nadc
  UShort_t      fAdc;  //  Adc

public:
  TAdcHit() {;}
  TAdcHit(UShort_t nadc, UShort_t adc);
  virtual      ~TAdcHit() {;}

  Int_t         GetNadc() const { return (Int_t)fNadc; }
  Int_t         GetAdc() const { return (Int_t)fAdc; }

  Int_t         SearchChannel() const { return gVME->SearchChannel(fNadc); }

  ClassDef(TAdcHit, 2) // AdcHit class (not original)
};

//______________________________________________________________________________

class TPommeEvent : public TObject {

private:
  Int_t         fNevent;  //  Nevent
  UInt_t        fNwires;  //  Nwires
  TClonesArray *fWires;   //->array with wire hits
  UInt_t        fNhits;   //  Nhits
  TClonesArray *fAdcHits; //->array with adc hits

public:
  TPommeEvent();
  virtual      ~TPommeEvent();

  Int_t         GetNEvent() const { return fNevent; }
  Int_t         GetNWires() const { return (Int_t)fNwires; }
  TClonesArray *WireHits() const { return fWires; }
  Int_t         GetNumOfWires() const { return fWires->GetEntriesFast(); }
  TChWire      *GetWireHit(Int_t iw) const { return (TChWire *)fWires->UncheckedAt(iw); }
  Int_t         GetNHits() const { return (Int_t)fNhits; }
  TClonesArray *AdcHits() const { return fAdcHits; }
  Int_t         GetNumOfAdcHits() const { return fAdcHits->GetEntriesFast(); }
  TAdcHit      *GetAdcHit(Int_t ia) const { return (TAdcHit *)fAdcHits->UncheckedAt(ia); }

  ClassDef(TPommeEvent, 2) // PommeEvent class (not original)
};

#endif
