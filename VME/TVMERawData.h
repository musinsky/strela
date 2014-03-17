// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    17 Mar 2014

#ifndef STRELA_TVMERawData
#define STRELA_TVMERawData

#include <TObject.h>

class TVMERawData : public TObject {

private:
  UInt_t        fDataWord;
  ULong_t       fNDataWords;
  Int_t         fNSpills;
  Int_t         fNEvents;   // !!!!!!all events += event from ehdr
  Int_t         fEventEHDR;
  Int_t         fEventMHDR;
  Bool_t        fFirstModule;
  Int_t         fAllEvents; // !!!!!!potom nepotrebne

public:
  // Data type status bits
  enum ETypeStatus {
    kSpill    = BIT(14), // Spill header/trailer
    kSpillEnd = BIT(15), // End of spill header/trailer
    kEvent    = BIT(16), // Event header/trailer
    kModule   = BIT(17)  // Module header/trailer
  };
  // Data type
  enum EDataType {
    kMHDR = 0x8, // Module header
    kMTRL = 0x9, // Module trailer
    kEHDR = 0xA, // Event header
    kETRL = 0xB, // Event trailer
    kSHDR = 0xC, // Spill header
    kSTRL = 0xD, // Spill trailer
    kSTAT = 0xE, // Status
    kRESE = 0xF  // Reserved
  };
  // TDC data type
  enum ETDCType {
    kTHDR = 0x2, // TDC header
    kTTRL = 0x3, // TDC trailer
    kTLD  = 0x4, // TDC leading time
    kTTR  = 0x5, // TDC trailing time
    kTERR = 0x6  // TDC error
  };

  TVMERawData();
  virtual ~TVMERawData();

  void          ReadFile(const char *fname);
  void          DecodeDataWord();
  void          DecodeSHDR();
  void          DecodeSTRL();
  void          DecodeEHDR();
  void          DecodeETRL();
  void          DecodeMHDR();
  void          DecodeMTRL();
  //  void          DecodeSTAT();
  //  void          DecodeRESE();
  void          DecodeOther();
  Bool_t        PrintDataWord(Int_t nlevel) const;

  void CheckIntegrity(ETypeStatus type, Bool_t status, const char *where);
  void CheckIntegrity2(ETypeStatus type, const char *where);

  ClassDef(TVMERawData, 1) // VMERawData class
};

#endif
