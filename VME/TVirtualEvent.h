// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Mar 2015

#ifndef STRELA_TVirtualEvent
#define STRELA_TVirtualEvent

#include <TClonesArray.h>
#include <TArrayI.h>

class TVirtualHit;

class TVirtualEvent : public TObject {

public:
  // indexing enumeration
  enum { kIdxEvent = BIT(14) };
  enum { kMaxMulti = 3, kNoneValue = -9999 };

  TVirtualEvent();
  virtual ~TVirtualEvent();

  Int_t         GetNHits() const { return fNHits; }
  TClonesArray *Hits() const { return fHits; }
  TVirtualHit  *GetHit(Int_t ih) const { return (TVirtualHit *)fHits->UncheckedAt(ih); }

  virtual void  Clear(Option_t *option = "");

protected:
  Int_t         fNHits; // number of hits
  TClonesArray *fHits;  //->array with hits

  TArrayI       fIdxHitChanLast;             //! positions of last hit by channel
  TArrayI       fIdxHitChanMulti[kMaxMulti]; //! positions of multi-th hit by channel

  void          IndexHitChanMulti();
  Int_t         GetIndexHit(Int_t ch, Int_t multi);
  Int_t         Multi(Int_t ch);

  ClassDef(TVirtualEvent, 1) // Event(virtual) class
};

#endif
