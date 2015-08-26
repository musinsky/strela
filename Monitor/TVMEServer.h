// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    27 Aug 2015

#ifndef STRELA_TVMEServer
#define STRELA_TVMEServer

#include <TString.h>

class TServerSocket;

class TVMEServer : public TObject {

public:
  // status bits
  enum {
    kVerbose   = BIT(14), // verbose mode
    kDataValid = BIT(15)  // validity (header and data)
  };

  TVMEServer();
  virtual ~TVMEServer();

  TServerSocket *GetServSock() const { return fServSock; }
  void           SetStoreDir(const char *dir) { fStoreDir = dir; }
  const char    *GetStoreDir() const { return fStoreDir.Data(); }
  void           Verbose(Bool_t set) { SetBit(kVerbose, set); }
  Bool_t         IsVerbose() const { return TestBit(kVerbose); }

  Bool_t         AreDataValid() const { return TestBit(kDataValid); }
  Int_t          GetDataCode() const { return fDataCode; }
  Long_t         GetDataFrom() const { return fDataFrom; }
  Long_t         GetDataTo() const { return fDataTo; }
  const char    *GetDataName() const { return fDataName.Data(); }

  void           OpenServer(Int_t port = 7503);
  void           RecvData();

private:
  TServerSocket *fServSock;  // server socket
  TString        fStoreDir;  // storage dir (default tmpfile)

  Int_t          fDataCode;  // code info
  Long_t         fDataFrom;  // data from
  Long_t         fDataTo;    // data to
  TString        fDataName;  // data name

  void           ResetHeader();
  void           DecodeHeader(const char *buffer, Int_t bsize);
  FILE          *OpenFile() const;
  void           PrintTime(Bool_t empty = kFALSE) const;

  ClassDef(TVMEServer, 1) // VMEServer class
};

#endif
