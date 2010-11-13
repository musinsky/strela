// Author: Jan Musinsky
// 23/06/2008

{
  if (gVME) {
    Printf("VME crate: '%s' already exists", gVME->GetName());
    return -1;
  }

  new TVME("seance 2008_06");
  new TModulePhTDC("module_0"); // 2y, 1y, trig, 0
  new TModuleTDC96("module_1"); // b3a, b2a, b1a
  new TModuleTDC96("module_2"); // 0, 3x2x, 1x3y
  gVME->ReDecodeChannels();
}
