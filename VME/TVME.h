// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    06 Dec 2013

#ifndef STRELA_TVME
#define STRELA_TVME

#include <TNamed.h>
#include <TObjArray.h>

class TVirtualModule;

class TVME : public TNamed {

private:
  TObjArray      *fModules;   //->list of all modules
  Int_t           fNChannels; //  number of channels
  Int_t          *fChannel;   //[fNChannels] array of all channels/nadc
  Int_t          *fIndexCha;  //[fNChannels] sorted index of channels
  Int_t          *fSortCha;   //[fNChannels] sorted array of channels
  static Bool_t   fgReDecode; // switch ReDecode

  void            DeleteChannels();

public:
  TVME();
  TVME(const char *name, const char *title = "VME Crate");
  virtual ~TVME();

  TObjArray      *Modules() const { return fModules; }
  TVirtualModule *GetModule(Int_t im) const { return (TVirtualModule *)fModules->UncheckedAt(im); }
  Int_t           GetNumOfEnabledModules() const { return fModules->GetEntries(); }
  Int_t           GetNChannelsFast() const { return fNChannels; }
  Int_t          *GetChannel() const { return fChannel; }
  static void     ReDecode(Bool_t enable) { fgReDecode = enable; }
  static Bool_t   IsReDecode() { return fgReDecode; }

  Int_t           GetNChannels() const;
  void            FirstChannelOfModules() const;
  void            ReDecodeChannels();
  Int_t           SearchChannel(Int_t nadc) const;
  const char     *GetChannelInfo(Int_t nadc) const;

  void            DecodeFile(const char *fname, Int_t ne = 0, Int_t imod = -1) const;

  ClassDef(TVME, 1) // VME class
};

R__EXTERN TVME *gVME;

#endif
