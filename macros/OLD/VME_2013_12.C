// Author: Jan Musinsky
// 06/12/2013

{
  if (gVME) {
    Printf("VME crate: '%s' already exists", gVME->GetName());
    return -1;
  }

  new TVME("seance 2013_12");

  new TModuleTDC64V(7);
  new TModuleTDC64V(9);
  new TModuleTDC64V(11);
  new TModuleTDC64V(13);
}
