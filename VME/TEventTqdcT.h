// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Mar 2015

#ifndef STRELA_TEventTqdcT
#define STRELA_TEventTqdcT

#include "TVirtualEvent.h"

class THitTqdcT;

class TEventTqdcT : public TVirtualEvent {

public:
  TEventTqdcT();
  virtual ~TEventTqdcT();

  Int_t         GetNHitsTqdcT() const { return GetNHits(); }
  THitTqdcT    *GetHitTqdcT(Int_t ih) const { return (THitTqdcT *)fHits->UncheckedAt(ih); }
  Int_t         GetTrigTime() const { return fTrigTime; }
  void          SetTrigTime(Int_t tt) { fTrigTime = tt; }

  virtual void  Clear(Option_t *option = "");
  virtual void  Print(Option_t *option = "") const;

  void          AddHitTqdcT(Int_t ch, Int_t t);

  Int_t         MultiTqdcT(Int_t ch) { return Multi(ch); }
  Int_t         T(Int_t ch, Int_t multi);

private:
  Int_t         fTrigTime; // trigger time

  ClassDef(TEventTqdcT, 1) // EventTqdcT
};

#endif
