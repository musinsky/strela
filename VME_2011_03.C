// Author: Jan Musinsky
// 09/03/2011

{
  if (gVME) {
    Printf("VME crate: '%s' already exists", gVME->GetName());
    return -1;
  }

  new TVME("seance 2011_03");
  new TModulePhTDC(0); // 1y, 2y, trig, 0
  new TModuleTDC96(1); // 3,4; 1,2; 5,6
  gVME->ReDecodeChannels();
}
