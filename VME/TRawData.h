// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    02 Dec 2013

#ifndef STRELA_TRawData
#define STRELA_TRawData

#include "Rtypes.h"

class TVirtualModule;

class TRawData {

private:
  UInt_t          fWord;
  FILE           *fFile;

  Int_t           fNWords;

  Int_t           fEvent;
  Int_t           fTdcId;
  TVirtualModule *fModule;

public:
  TRawData();
  virtual     ~TRawData();

  FILE        *GetFile() const { return fFile; }

  void         ParseFile(const char *fname);
  void         FindType();

  void         DecodeTHDR();
  void         DecodeTTRL();
  void         DecodeTLD();
  void         DecodeTTR();
  void         DecodeTERR();

  void         DecodeMHDR();
  void         DecodeMTRL();
  void         DecodeEHDR();
  void         DecodeETRL();
  void         DecodeSHDR();
  void         DecodeSTRL();
  void         DecodeSTAT();
  void         DecodeRESE();
  void         DecodeOther();
  void         PrintWord(Int_t level, Bool_t space = kTRUE);

  ClassDef(TRawData, 1) // RawData class
};

#endif
