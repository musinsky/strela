// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    13 Apr 2014

#ifndef STRELA_TVirtualModule
#define STRELA_TVirtualModule

#include <TObject.h>

class TVirtualModule : public TObject {

public:
  TVirtualModule();
  virtual ~TVirtualModule();

  Int_t          GetId() const { return fId; }
  Int_t          GetSlot() const { return fSlot; }
  Int_t          GetNChips() const { return fNChips; }
  Int_t          GetChipNChannels() const { return fChipNChannels; }
  Int_t          GetModuleNChannels() const { return fNChips*fChipNChannels; }
  Int_t          GetFirstChannel() const { return fFirstChannel; }
  void           SetFirstChannel(Int_t first) { fFirstChannel = first; }

  virtual const char *GetTitle() const;

  virtual Int_t  MapChannel(Int_t tdcid, Int_t tdcch) const;
  virtual Bool_t GetChannelIdCh(Int_t ch, Int_t &tdcid, Int_t &tdcch) const;
  virtual void   ConnectorChannels(Int_t con, Int_t *pins, Option_t *option = "") const;
  virtual Int_t  DecodeTDCChannel(UInt_t word) const;

protected:
  Int_t          fId;            // Id module
  Int_t          fSlot;          // unique slot (gate) number in VME
  Int_t          fNChips;        // number of chips
  Int_t          fChipNChannels; // number of channels in chip
  Int_t          fFirstChannel;  // first channel of module

  virtual void   VMEModule(Int_t slot);

  ClassDef(TVirtualModule, 1) // Module(virtual) class
};

#endif
