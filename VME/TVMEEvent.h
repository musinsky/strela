// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Apr 2014

#ifndef STRELA_TVMEEvent
#define STRELA_TVMEEvent

#include <TClonesArray.h>
#include <TArrayI.h>

class TTDCHit;

const Int_t kMaxMulti = 3;

class TVMEEvent : public TObject {

public:
  // status bit used for indexing (not stored)
  enum { kNextEvent = BIT(14) };

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
  static Int_t  GetTrigOffset() { return fgTrigOffset; }
  static void   SetTrigInfo(Int_t channel, Int_t offset);
  Int_t         Multi(Int_t ch);
  Int_t         Time(Int_t ch, Int_t multi);
  Int_t         Delta(Int_t ch, Int_t multi);

private:
  Int_t         fEvent;    // event number in one spill
  Int_t         fNTDCHits; // number of TDC hits
  TClonesArray *fTDCHits;  //->array with TDC hits

  TArrayI       fIdxTDCHitChanLast;             //! positions of last TDC hit by channel
  TArrayI       fIdxTDCHitChanMulti[kMaxMulti]; //! positions of multi-th TDC hit by channel
  static Int_t  fgTrigChannel;                  //! number of Trig channel
  static Int_t  fgTrigOffset;                   //! offset of Trig channel

  void          IndexTDCHitChanMulti();
  Int_t         GetIndexTDCHit(Int_t ch, Int_t multi);

  ClassDef(TVMEEvent, 1) // VMEEvent
};

#endif
