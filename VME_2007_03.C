// Author: Jan Musinsky
// 23/06/2008

{
  if (gVME) {
    Printf("VME crate: '%s' already exists", gVME->GetName());
    return -1;
  }

  new TVME("seance 2007_03");
  new TModulePhTDC("module_0"); // 1y, 1x, trig, 2y
  new TModulePhTDC("module_1"); // 0, 0, 3y, 3x
  new TModulePhTDC("module_2"); // by1, bx1, 5, red
  gVME->ReDecodeChannels();
}
