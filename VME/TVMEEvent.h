// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    28 Mar 2014

#ifndef STRELA_TVMEEvent
#define STRELA_TVMEEvent

#include <TClonesArray.h>

class TTDCHit : public TObject {

public:
  TTDCHit() : TObject(), fChannel(0), fTime(0), fDelta(0) {;}
  TTDCHit(Int_t ch, Int_t tld);
  virtual ~TTDCHit() {;}

  Int_t         GetChannel() const { return fChannel; }
  Int_t         GetTime() const { return fTime; }
  Int_t         GetDelta() const { return fDelta; }

  Int_t         GetTLD() const { return fTime; }
  Int_t         GetTTR() const { return fDelta ? fDelta + fTime : 0; }
  void          AddDelta(Int_t ttr);

private:
  Int_t         fChannel; // channel
  Int_t         fTime;    // leading time
  Int_t         fDelta;   // delta time

  ClassDef(TTDCHit, 1) // TTDCHit
};

//______________________________________________________________________________

class TVMEEvent : public TObject {

public:
  TVMEEvent();
  virtual ~TVMEEvent();

  Int_t         GetEvent() const { return fEvent; }
  Int_t         GetNTDCHits() const { return fNTDCHits; }
  TClonesArray *TDCHits() const { return fTDCHits; }

  virtual void  Clear(Option_t *option = "");
  virtual void  Print(Option_t *option = "") const;

  Int_t         GetNumOfTDCHits() const { return fTDCHits->GetEntriesFast(); }
  TTDCHit      *GetTDCHit(Int_t ih) const { return (TTDCHit *)fTDCHits->UncheckedAt(ih); }
  void          AddTDCHit(Int_t ch, Int_t tdc, Bool_t lead);

private:
  Int_t         fEvent;    // event number
  Int_t         fNTDCHits; // number of TDC hits
  TClonesArray *fTDCHits;  //->array with TDC hits

  Int_t        *fPosTDCHitChan; //! array with position of last TDC hit by channel

  ClassDef(TVMEEvent, 1) // TVMEEvent
};

#endif
