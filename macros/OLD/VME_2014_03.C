// Author: Jan Musinsky
// 30/06/2014

{
  if (gVME) {
    Printf("VME crate: '%s' already exists", gVME->GetName());
    return -1;
  }

  new TVME("seance 2014_03");

  new TModuleTDC64V(7);
  new TModuleTDC64V(9);
  new TModuleTDC64V(11);
  new TModuleTDC64V(13);

  TEventTdc::SetTrigInfo(58, 8000);
}
