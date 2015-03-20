// Author: Jan Musinsky
// 19/02/2015

{
  if (gVME) {
    Printf("VME crate: '%s' already exists", gVME->GetName());
    return -1;
  }

  new TVME("seance 2015_02");

  new TModuleTDC64V(7);
  new TModuleTDC64V(9);
  new TModuleTDC64V(11);

  new TModuleTQDC16(15);
  new TModuleTQDC16(17);
  new TModuleTQDC16(19);

  TEventTdc::SetTrigInfo(58, 6000);
}
