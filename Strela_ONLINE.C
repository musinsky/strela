#ifndef __CINT__
#include <TROOT.h>

void Strela_ONLINE()
#endif
{
  if (gStrela) return;
  new TStrela("strela");
  gROOT->Macro("VME_2016_11.C");
  TStrawCham::ShiftAdc(TEventTdc::GetTrigOffset());  // 6000
  TStrawCham::TrigNadc(TEventTdc::GetTrigChannel()); // 58
  gStrela->SetSeance("2016_11");
  gStrela->Init();

  TStrawCham::Tracking(0);
}
