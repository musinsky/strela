// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    15 Jun 2016

#ifndef STRELA_TVMERawData
#define STRELA_TVMERawData

#include <TBits.h>
#include <TString.h>

class TTree;

class TEventTdc;
class TEventTqdcT;
class TEventTqdcQ;
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
    kTTCM   = 0x0A, // TTCM (not used)
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
  // TQDC data type
  enum ETQDCType {
    kTQCHI = 0x0, // TQDC input counters high bits
    kTQCLO = 0x1, // TQDC input counters low bits
    kTQHDR = 0x2, // TQDC header
    kTQTRL = 0x3, // TQDC trailer
    kTQDC4 = 0x4, // TQDC ADC timestamp or TDC data
    kTQDC5 = 0x5, // TQDC TDC or ADC data
    kTQERR = 0x6  // TQDC error
  };
  // TQDC options
  enum {
    kADCBits = 14 // ADC sampling resolution
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
  // TDC decoding
  void          DecodeDataTDC();
  void          DecodeTHDR();
  void          DecodeTTRL();
  void          DecodeTLD();
  void          DecodeTTR();
  void          DecodeTERR();
  // TQDC decoding
  void          DecodeDataTQDC();
  void          DecodeTQCHI();
  void          DecodeTQCLO();
  void          DecodeTQHDR();
  void          DecodeTQTRL();
  void          DecodeTQDC4();
  void          DecodeTQDC5();
  void          DecodeTQERR();

  void          CheckIntegrity(ETypeStatus type, Bool_t status, const char *where);
  void          CheckIntegrity2(ETypeStatus type, const char *where);

  Int_t			GetNSpills() const { return fNSpills; }
  Int_t			GetNEvents() const { return fNEvents; }
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
  TEventTdc      *fEventTdc;
  TEventTqdcT    *fEventTqdcT;
  TEventTqdcQ    *fEventTqdcQ;
  TVirtualModule *fModule;
  TString         fTreeFileName;

  ClassDef(TVMERawData, 1) // VMERawData class
};

#endif
