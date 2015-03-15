// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    15 Mar 2015

#ifndef STRELA_TTQDCEvent
#define STRELA_TTQDCEvent

#include <TClonesArray.h>

class TTQDCHitT;
class TTQDCHitQ;

class TTQDCEvent : public TObject {

public:
  TTQDCEvent();
  virtual ~TTQDCEvent();

  Int_t         GetEvent() const { return fEvent; }
  void          SetEvent(Int_t ev) { fEvent = ev; }
  Int_t         GetTrigTime() const { return fTrigTime; }
  void          SetTrigTime(Int_t tt) { fTrigTime = tt; }

  virtual void  Clear(Option_t *option = "");
  virtual void  Print(Option_t *option = "") const;

  // TQDCHitT
  Int_t         GetNHitsT() const { return fNHitsT; }
  TClonesArray *HitsT() const { return fHitsT; }

  Int_t         GetNumOfHitsT() const { return fHitsT->GetEntriesFast(); }
  TTQDCHitT    *GetHitT(Int_t ih) const { return (TTQDCHitT *)fHitsT->UncheckedAt(ih); }
  void          AddHitT(Int_t ch, Int_t t);

  // TQDCHitQ
  Int_t         GetNHitsQ() const { return fNHitsQ; }
  TClonesArray *HitsQ() const { return fHitsQ; }

  Int_t         GetNumOfHitsQ() const { return fHitsQ->GetEntriesFast(); }
  TTQDCHitQ    *GetHitQ(Int_t ih) const { return (TTQDCHitQ *)fHitsQ->UncheckedAt(ih); }
  void          AddHitQ(Int_t ch, Int_t ts, Bool_t adc);
  void          NextSampleHitQ(Int_t ch, Int_t sample) const;

private:
  Int_t         fEvent;    // event number in one spill
  Int_t         fTrigTime; // trigger time

  Int_t         fNHitsT;   // number of TQDC time hits
  TClonesArray *fHitsT;    //->array with TQDC time hits

  Int_t         fNHitsQ;   // number of TQDC charge hits
  TClonesArray *fHitsQ;    //->array with TQDC charge hits

  TTQDCHitQ    *fHitQLast;      //! last TQDC charge hit
  Int_t         fHitQTimeStamp; //! ADC timestamp

  ClassDef(TTQDCEvent, 1) // TQDCEvent
};

#endif
