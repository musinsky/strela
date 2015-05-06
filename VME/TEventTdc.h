// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Mar 2015

#ifndef STRELA_TEventTdc
#define STRELA_TEventTdc

#include "TVirtualEvent.h"

class THitTdc;

class TEventTdc : public TVirtualEvent {

public:
  TEventTdc();
  virtual ~TEventTdc();

  Int_t         GetNHitsTdc() const { return GetNHits(); }
  THitTdc      *GetHitTdc(Int_t ih) const { return (THitTdc *)fHits->UncheckedAt(ih); }
  Int_t         GetTrigTime() const { return fTrigTime; }
  static Int_t  GetTrigChannel() { return fgTrigChannel; }
  static Int_t  GetTrigOffset() { return fgTrigOffset; }

  virtual void  Clear(Option_t *option = "");
  virtual void  Print(Option_t *option = "") const;

  void          AddHitTdc(Int_t ch, Int_t tld);
  void          AddHitTdcCheck(Int_t ch, Int_t tdc, Bool_t ld);
  static void   SetTrigInfo(Int_t channel, Int_t offset);

  Int_t         MultiTdc(Int_t ch) { return Multi(ch); }
  Int_t         Time(Int_t ch, Int_t multi);
  Int_t         Delta(Int_t ch, Int_t multi);

private:
  Int_t         fTrigTime; // trigger time
  static Int_t  fgTrigChannel; //! number of Trig channel
  static Int_t  fgTrigOffset;  //! offset of Trig time

  ClassDef(TEventTdc, 1) // EventTdc
};

#endif
