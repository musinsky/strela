// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    17 Feb 2015

#ifndef STRELA_TVMERawData
#define STRELA_TVMERawData

#include <TBits.h>

class TTree;

class TVMEEvent;
class TVirtualModule;

class TVMERawData : public TObject {

public:
  // Data type status bits
  enum ETypeStatus {
    kSpill    = BIT(14), // Spill header/trailer
    kSpillEnd = BIT(15), // Spill (end of data)
    kEvent    = BIT(16), // Event header/trailer
    kModule   = BIT(17), // Module header/trailer
    kData     = BIT(18)  // Data (specifying by module)
  };
  // Data options
  enum {
    kWrongEvent = BIT(23) // Wrong event
  };
  // Module Id
  enum EModuleId {
    kPhTDC  = 0x04, // TDC (obsolete)
    kTDC96  = 0x05, // TDC (obsolete)
    kTQDC16 = 0x09, // TQDC
    kTRIG   = 0x0A, // Logic (not used)
    kTDC64V = 0x10  // TDC
  };
  // Common data type
  enum ECommonType {
    kMHDR = 0x8, // Module header
    kMTRL = 0x9, // Module trailer
    kEHDR = 0xA, // Event header
    kETRL = 0xB, // Event trailer
    kSHDR = 0xC, // Spill header
    kSTRL = 0xD, // Spill trailer
    kSTAT = 0xE, // Status
    kPADD = 0xF  // Padding
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

  void          Reset();
  void          MakeTree(const char *fname);
  void          DecodeFile(const char *fname, Bool_t tree = kTRUE);
  void          DecodeDataWord();
  void          DecodeSHDR();
  void          DecodeSTRL();
  void          DecodeEHDR();
  void          DecodeETRL();
  void          DecodeMHDR();
  void          DecodeMTRL();
  void          DecodeSTAT();
  void          DecodePADD();
  void          DecodeData();
  void          DecodeDataTDC();
  //  void          DecodeDataTQDC();
  //  void          DecodeDataTTCM();

  // TDC decoding
  void          DecodeTHDR();
  void          DecodeTTRL();
  void          DecodeTLD();
  void          DecodeTTR();
  void          DecodeTERR();

  void          CheckIntegrity(ETypeStatus type, Bool_t status, const char *where);
  void          CheckIntegrity2(ETypeStatus type, const char *where);

  TBits        *GetPrintType() const { return fPrintType; }
  Bool_t        PrintDataType(Int_t nlevel) const;
  const char   *GetTreeFileName() const { return fTreeFileName.Data(); }
  void          SetTreeFileName(const char *name) { fTreeFileName = name; }

private:
  UInt_t        fDataWord;   // one word of data
  UInt_t        fDataType;   // type of data
  ULong_t       fNDataWords; // all words
  Int_t         fNSpills;    // all spills
  Int_t         fNEvents;    // all events
  Int_t         fEventEHDR;  // event number in EHDR
  Int_t         fEventMHDR;  // event number in MHDR
  Int_t         fModuleId;   // module Id from MHDR
  TBits        *fPrintType;  // table of bits to print type of data

  TTree          *fTree;
  TVMEEvent      *fVMEEvent;
  TVirtualModule *fModule;
  TString         fTreeFileName;

  ClassDef(TVMERawData, 1) // VMERawData class
};

#endif
