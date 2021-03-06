// Author: Jan Musinsky
// 23/06/2008

{
  if (gVME) {
    Printf("VME crate: '%s' already exists", gVME->GetName());
    return -1;
  }

  new TVME("seance 2008_06");
  TVME::ReDecode(kTRUE);

  new TModulePhTDC(0); // 2y, 1y, trig, 0
  new TModuleTDC96(1); // b3a, b2a, b1a
  new TModuleTDC96(2); // 0, 3x2x, 1x3y
}
