// Author: Jan Musinsky
// 12/07/2010

#include <TFile.h>
#include <TROOT.h>

#include "TStrawTracker.h"

#ifndef MNULL
#define MNULL -9999
#endif

class THit : public TObject {
private:
  Int_t         fChannel; //  Channel
  Int_t         fTdc;     //  TDC

public:
  THit() {;}
  THit(const Int_t ch, const Int_t t);
  virtual      ~THit() {;}

  Int_t      GetChannel() const { return fChannel; }
  Int_t      GetTDC() const { return fTdc; }

  ClassDef(THit, 1) // THit class
};

ClassImp(THit)

THit::THit(const Int_t ch, const Int_t t)
{
  fChannel = ch;
  fTdc = t;
}

//______________________________________________________________________________

class TEvent : public TObject {
private:
  Int_t         fNhits; //  Nhits
  TClonesArray *fHits;  //->array with TDC hits

public:
  TEvent();
  virtual      ~TEvent();

  Int_t         GetNHits() const { return fNhits; }
  TClonesArray *Hits() const { return fHits; }
  Int_t         GetNumOfHits() const { return fHits->GetEntriesFast(); }
  THit         *Hit(Int_t ih) const { return (THit *)fHits->UncheckedAt(ih); }

  virtual void  Clear(Option_t *option = "");
  void          AddHit(const Int_t chh, const Int_t th);

  Int_t         FindChannel(Int_t chan) const;
  Bool_t        Ch(Int_t ch) const { return
      (FindChannel(ch) == -1) ? kFALSE : kTRUE; }
  Int_t         T(Int_t ch) const;

  ClassDef(TEvent, 1) // TEvent class
};

ClassImp(TEvent)

TEvent::TEvent()
{
  //  Info("TEvent", "Default constructor");
  fNhits = 0;
  fHits  = new TClonesArray("THit", 4);
}

TEvent::~TEvent()
{
  Info("~TEvent", "Destructor");
  Clear("C");
  if (fHits) fHits->Delete();
  delete fHits;
  fHits = 0;
}

void TEvent::Clear(Option_t *option)
{
  fNhits = 0;
  fHits->Clear(option);
}

void TEvent::AddHit(const Int_t chh, const Int_t th)
{
  TClonesArray &hits = *fHits;
  new(hits[fNhits++]) THit(chh, th);
}

Int_t TEvent::FindChannel(Int_t chan) const
{
  // !!! find only first hit with channel (no multiple hits) !!!
  for (Int_t ich = 0; ich < fNhits; ich++)
    if (Hit(ich)->GetChannel() == chan) return ich;
  return -1;
}

Int_t TEvent::T(Int_t ch) const
{
  Int_t ichan = FindChannel(ch);
  if (ichan == -1) return MNULL;
  return Hit(ichan)->GetTDC();
}
