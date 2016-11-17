// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    17 Nov 2016

#ifndef STRELA_TVME
#define STRELA_TVME

#include <TNamed.h>
#include <TObjArray.h>

class TVirtualModule;

class TVME : public TNamed {

public:
  TVME();
  TVME(const char *name, const char *title = "VME DAQ");
  virtual ~TVME();

  TObjArray      *Modules() const { return fModules; }
  TVirtualModule *GetModule(Int_t slot) const { return (TVirtualModule *)fModules->UncheckedAt(slot); }
  Int_t           GetNumOfEnabledModules() const { return fModules->GetEntries(); }
  Int_t           GetNChannelsMSC() const { return fNChannelsMSC; }
  Int_t           GetNChannelsTQDC() const { return fNChannelsTQDC; }
  Int_t           GetNChannelsTDC() const { return fNChannels; }
  Int_t           GetNChannels() const { return fNChannels; }
  Int_t          *GetChannel() const { return fChannel; }
  static void     ReDecode(Bool_t enable) { fgReDecode = enable; }
  static Bool_t   IsReDecode() { return fgReDecode; }

  void            ReDecodeChannels();
  Int_t           SearchChannel(Int_t nadc) const;
  const char     *GetChannelInfo(Int_t nadc) const;

private:
  TObjArray      *fModules;   //->list of all modules
  Int_t           fNChannelsMSC;  // number of MSC  channels
  Int_t           fNChannelsTQDC; // number of TQDC channels
  Int_t           fNChannels;     // number of TDC  channels
  Int_t          *fChannel;   //[fNChannels] array of all TDC channels/nadc
  Int_t          *fIndexCha;  //[fNChannels] sorted index of TDC channels
  Int_t          *fSortCha;   //[fNChannels] sorted array of TDC channels
  static Bool_t   fgReDecode; // switch ReDecode

  void            DeleteChannels();
  void            CountChannels();

  ClassDef(TVME, 1) // VME class
};

R__EXTERN TVME *gVME;

#endif
