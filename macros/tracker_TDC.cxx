// Author: Jan Musinsky
// 01/08/2011

#include <TClonesArray.h>

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

  Int_t         GetChannel() const { return fChannel; }
  Int_t         GetTDC() const { return fTdc; }

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

  Int_t         CountChannel(Int_t chan) const;
  Int_t         FindChannel(Int_t chan) const;
  Int_t         FindChannel_M(Int_t chan, Int_t multi) const;
  Bool_t        Ch(Int_t ch) const { return (FindChannel(ch) == -1) ? kFALSE : kTRUE; }
  Bool_t        Ch_M(Int_t ch, Int_t m = 0) const { return (FindChannel_M(ch, m) == -1) ? kFALSE : kTRUE; }
  Int_t         T(Int_t ch) const;
  Int_t         T_M(Int_t ch, Int_t m = 0) const;

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

Int_t TEvent::CountChannel(Int_t chan) const
{
  // count all hits per one channel
  Int_t count = 0;
  for (Int_t ich = 0; ich < fNhits; ich++)
    if (Hit(ich)->GetChannel() == chan)
      count++;

  return count;
}

Int_t TEvent::FindChannel(Int_t chan) const
{
  // find only if once hit per channel
  Int_t found = -1, count = 0;
  for (Int_t ich = 0; ich < fNhits; ich++) {
    if (Hit(ich)->GetChannel() == chan) {
      found = ich;
      count++;
    }
  }
  if (count == 1) return found;
  else return -1;
}

Int_t TEvent::FindChannel_M(Int_t chan, Int_t multi) const
{
  // find multiple hits per channel
  Int_t count = 0;
  for (Int_t ich = 0; ich < fNhits; ich++) {
    if (Hit(ich)->GetChannel() == chan) {
      if (count == multi) return ich;
      count++;
    }
  }
  return -1;
}

Int_t TEvent::T(Int_t ch) const
{
  Int_t ichan = FindChannel(ch);
  if (ichan == -1) return MNULL;
  return Hit(ichan)->GetTDC();
}

Int_t TEvent::T_M(Int_t ch, Int_t m) const
{
  Int_t ichan = FindChannel_M(ch, m);
  if (ichan == -1) return MNULL;
  return Hit(ichan)->GetTDC();
}
