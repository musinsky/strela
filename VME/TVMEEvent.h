// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    16 Apr 2014

#ifndef STRELA_TVMEEvent
#define STRELA_TVMEEvent

#include <TClonesArray.h>
#include <TArrayI.h>

class TTDCHit : public TObject {

public:
  TTDCHit() : TObject(), fChannel(0), fTime(0), fDelta(0) {;}
  TTDCHit(Int_t ch, Int_t tld);
  virtual ~TTDCHit() {;}

  Int_t         GetChannel() const { return fChannel; }
  Int_t         GetTime() const { return fTime; }
  Int_t         GetDelta() const { return fDelta; }
  void          SetDelta(Int_t ttr);
  Int_t         GetTLD() const { return fTime; }
  Int_t         GetTTR() const { return fDelta ? fTime + fDelta : 0; }

private:
  UShort_t      fChannel; // channel
  UShort_t      fTime;    // leading time
  UShort_t      fDelta;   // delta time

  ClassDef(TTDCHit, 1) // TDCHit
};

//______________________________________________________________________________

const Int_t kMaxMulti = 3;

class TVMEEvent : public TObject {

public:
  TVMEEvent();
  virtual ~TVMEEvent();

  Int_t         GetEvent() const { return fEvent; }
  void          SetEvent(Int_t ev) { fEvent = ev; }
  Int_t         GetNTDCHits() const { return fNTDCHits; }
  TClonesArray *TDCHits() const { return fTDCHits; }

  virtual void  Clear(Option_t *option = "");
  virtual void  Print(Option_t *option = "") const;

  Int_t         GetNumOfTDCHits() const { return fTDCHits->GetEntriesFast(); }
  TTDCHit      *GetTDCHit(Int_t ih) const { return (TTDCHit *)fTDCHits->UncheckedAt(ih); }
  void          AddTDCHit(Int_t ch, Int_t tld);
  void          AddTDCHitCheck(Int_t ch, Int_t tdc, Bool_t ld);

  static Int_t  GetTrigChannel() { return fgTrigChannel; }
  static void   SetTrigChannel(Int_t ch);

private:
  Int_t         fEvent;    // event number in one spill
  Int_t         fNTDCHits; // number of TDC hits
  TClonesArray *fTDCHits;  //->array with TDC hits

  TArrayI       fIdxTDCHitChan;                 //! positions of TDC hit (last) by channel
  TArrayI       fIdxTDCHitChanMulti[kMaxMulti]; //! positions of TDC hit [first, second, ...] by channel
  static Int_t  fgTrigChannel;                  //! number of Trig channel

  ClassDef(TVMEEvent, 1) // VMEEvent
};

#endif
