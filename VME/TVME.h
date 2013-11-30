// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    30 Nov 2013

#ifndef STRELA_TVME
#define STRELA_TVME

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif
#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

class TVirtualModule;

class TVME : public TNamed {

private:
  TObjArray   *fModules;   //->list of all modules
  Int_t        fNChannels; //  number of channels
  Int_t       *fChannel;   //[fNChannels] array of all channels/nadc
  Int_t       *fIndexCha;  //[fNChannels] sorted index of channels
  Int_t       *fSortCha;   //[fNChannels] sorted array of channels

  void         DeleteChannels();

public:
  TVME();
  TVME(const char *name, const char *title = "VME Crate");
  virtual     ~TVME();

  TObjArray      *Modules() const { return fModules; }
  TVirtualModule *FindModule(const char *name) const { return (TVirtualModule *)fModules->FindObject(name); }
  Int_t        GetNChannelsFast() const { return fNChannels; }
  Int_t       *GetChannel() const { return fChannel; }

  Int_t        GetNumOfModules() const;
  Int_t        GetNChannels() const;
  Int_t        FirstChannelOfModule(const TVirtualModule *mod) const;
  void         ReDecodeChannels();
  Int_t        SearchChannel(Int_t nadc) const;
  const char  *GetChannelInfo(Int_t nadc) const;

  void         DecodeFile(const char *fname, Int_t ne = 0, Int_t imod = -1) const;
  void         RawParser(const char *fname) const;

  ClassDef(TVME, 1) // VME class
};

R__EXTERN TVME *gVME;

#endif
