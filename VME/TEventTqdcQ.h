// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Mar 2015

#ifndef STRELA_TEventTqdcQ
#define STRELA_TEventTqdcQ

#include <TVirtualEvent.h>

class THitTqdcQ;

class TEventTqdcQ : public TVirtualEvent {

public:
  TEventTqdcQ();
  virtual ~TEventTqdcQ();

  Int_t         GetNHitsTqdcQ() const { return GetNHits(); }
  THitTqdcQ    *GetHitTqdcQ(Int_t ih) const { return (THitTqdcQ *)fHits->UncheckedAt(ih); }

  virtual void  Clear(Option_t *option = "");
  virtual void  Print(Option_t *option = "") const;

  void          AddHitTqdcQ(Int_t ch, Int_t ts, Bool_t adc);
  void          NextSampleHit(Int_t ch, Int_t sample) const;

  Int_t         MultiTqdcQ(Int_t ch) { return Multi(ch); }
  Int_t         Q(Int_t ch, Int_t multi);
  Int_t         TimeStamp(Int_t ch, Int_t multi);
  Int_t         Sample(Int_t ch, Int_t multi, Int_t samp);

private:
  THitTqdcQ    *fLastHit; //! last hit
  Int_t         fTrigTs;  //! tmp trig timestamp

  ClassDef(TEventTqdcQ, 1) // EventTqdcQ
};

#endif
