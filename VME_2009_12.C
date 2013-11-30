// Author: Jan Musinsky
// 23/06/2010

{
  if (gVME) {
    Printf("VME crate: '%s' already exists", gVME->GetName());
    return -1;
  }

  new TVME("seance 2009_12");
  new TModulePhTDC(0); // 1y, 2y, trig, 0
  new TModuleTDC96(1); // 3,4; 1,2; 5,6
  new TModuleTDC96(2); // 1x,3y; 3x,2x; 0
  gVME->ReDecodeChannels();
}
