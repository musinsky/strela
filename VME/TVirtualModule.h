// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    04 Dec 2013

#ifndef STRELA_TVirtualModule
#define STRELA_TVirtualModule

#include <TObjArray.h>

class TMultiHit;
class TGemEvent;

class TVirtualModule : public TObject {

protected:
  Int_t          fId;            // Id module
  Int_t          fSlot;          // unique slot (gate) number in VME
  Int_t          fNChips;        // number of chips
  Int_t          fChipNChannels; // number of channels in chip
  TObjArray     *fMultiHits;     //->array of multihit

public:
  TVirtualModule();
  TVirtualModule(Int_t slot);
  virtual ~TVirtualModule();

  Int_t          GetId() const { return fId; }
  Int_t          GetSlot() const { return fSlot; }
  Int_t          GetNChips() const { return fNChips; }
  Int_t          GetChipNChannels() const { return fChipNChannels; }
  Int_t          GetModuleNChannels() const { return fNChips*fChipNChannels; }
  TObjArray     *MultiHits() const { return fMultiHits; }
  TMultiHit     *GetMultiHit(Int_t i) const { return (TMultiHit *)fMultiHits->UncheckedAt(i); }

  virtual const char *GetName() const { return ClassName(); }
  virtual const char *GetTitle() const;

  virtual Int_t  MapChannel(Int_t tdcid, Int_t tdcch) const;
  virtual Bool_t GetChannelIdCh(Int_t ch, Int_t &tdcid, Int_t &tdcch) const;
  virtual void   ConnectorChannels(Int_t con, Int_t *pins, Option_t *option = "") const;
  virtual Int_t  DecodeChannel(UInt_t word) const;
  virtual void   MultiHitAdd(Int_t nadc, Int_t time, Bool_t lead);
  virtual void   MultiHitClear() const;
  virtual void   MultiHitIdentify(TGemEvent *event) const;

  ClassDef(TVirtualModule, 1) // Module(virtual) class
};

#endif
