// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    04 Dec 2013

#ifndef STRELA_TRawData
#define STRELA_TRawData

#include <Rtypes.h>
#include <TObject.h>


class TTree;
class TVirtualModule;
class TGemEvent;

class TRawData : public TObject {

private:
  UInt_t          fWord;
  FILE           *fFile;

  Int_t           fNWords;
  Int_t           fNSpills;
  Int_t           fNEvent;

  //  Bool_t          fSpillEnd;

  Int_t           fEvent;
  Int_t           fTdcId;
  TVirtualModule *fModule;

  TTree *fTree;
  TGemEvent *fGemEvent;

public:
  TRawData();
  virtual     ~TRawData();

  FILE        *GetFile() const { return fFile; }

  void         ParseFile(const char *fname); // static ?!
  void         FindType();
  void         FindType2();

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
  void         DecodeStatus();
  void         DecodeReserved();
  void         DecodeOther();
  void         PrintWord(Int_t level, Bool_t space = kTRUE);

  ClassDef(TRawData, 1) // RawData class
};

#endif
