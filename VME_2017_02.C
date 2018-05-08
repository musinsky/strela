// Author: Jan Musinsky
// 08/05/2018

{
  if (gVME) {
    Printf("VME crate: '%s' already exists", gVME->GetName());
    return -1;
  }

  new TVME("seance 2017_02");

  new TModuleTDC64V(7);
  new TModuleTDC64V(9);
  new TModuleTDC64V(11);

  new TModuleMSC16V(13);

  new TModuleTQDC16(15);
  new TModuleTQDC16(17);
  new TModuleTQDC16(19);

  TEventTdc::SetTrigInfo(58, 6000);
}
