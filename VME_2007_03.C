// Author: Jan Musinsky
// 23/06/2008

{
  if (gVME) {
    Printf("VME crate: '%s' already exists", gVME->GetName());
    return -1;
  }

  new TVME("seance 2007_03");
  new TModulePhTDC(0); // 1y, 1x, trig, 2y
  new TModulePhTDC(1); // 0, 0, 3y, 3x
  new TModulePhTDC(2); // by1, bx1, 5, red
  gVME->ReDecodeChannels();
}
