// Author: Jan Musinsky
// 23/06/2008

#include <TList.h>
#include <TSystem.h>

#include "TVME.h"
#include "TVirtualModule.h"

TList *list = new TList();

const Int_t kMaxCh = 8;    // max. number of channels in one layer
const Int_t kMaxPins = 16; // number of pins in one connector/cable
const Double_t kDelZ = 2.6, kDelta = 4.2, kRange = 2.1; // geometry

class TLayer : public TNamed {
private:
  Int_t        fId;
  Int_t        fCh[kMaxCh];
  Int_t        fLast;
  static Int_t fCounter;
  Int_t        fTracker;
  Double_t     fZ;
  Int_t        fDirect;

public:
  TLayer(const char *name, const char *title = "");
  virtual     ~TLayer() {;}
  virtual void Print(Option_t *option = "") const;

  Int_t        GetId() const { return fId; }
  Int_t        GetCh(Int_t ch) const { return (ch <= fLast) ? fCh[ch] : -2; }
  void         SetCh(Int_t ch, Int_t nadc);
  void         AddCh(Int_t nadc) { SetCh(fLast + 1, nadc); }
  Int_t        GetLast() const { return fLast; }
  static void  ResetCounter() { fCounter = 1; }
  Int_t        GetTrackar() const { return fTracker; }
  void         SetTracker(Int_t t) { fTracker = t; }
  Double_t     GetZ() const { return fZ; }
  void         SetZ(Double_t z) { fZ = z; }
  Int_t        GetDirect() const { return fDirect; }
  void         SetDirect(Int_t d) { fDirect = d; }

  ClassDef(TLayer, 0)
};

Int_t TLayer::fCounter = 1; // Numb start from 1

TLayer::TLayer(const char *name, const char *title)
  : TNamed(name, title)
{
  fId   = fCounter++;
  fLast = -1; // must be -1
  for (Int_t r = 0; r < kMaxCh; r++) fCh[r] = -1;
  fTracker = 0;
  fZ       = 0.0;
  fDirect  = 1;
}

void TLayer::Print(Option_t *option) const
{
  TString opt = option;
  const char *comma;

  // channels
  if (opt.Contains("channels")) {
    if (opt.Contains("first")) {
      Printf("--\n-- %s\n--\n", gVME->GetName());
      gSystem->Exec("more -99 sql/head_channels.sql");
      Printf("\n\n");
      Printf("INSERT INTO `channels` (`Nadc`, `DetNumb`, `DetChan`, `T0`, `TMax`) VALUES");
    }

    for (Int_t i = 0; i <= fLast; i++) {
      comma = ",";
      if (opt.Contains("last") && (i == fLast)) comma = ";";
      Printf("(%5d /*%4d*/, %3d /*%10s*/, %3d, %4d, %4d)%s",
             GetCh(i), gVME->SearchChannel(GetCh(i)), fId,
             GetName(), i + 1, 0, 0, comma);
    }
    return;
  }

  // layers
  if (opt.Contains("layers")) {
    if (opt.Contains("first")) {
      Printf("--\n-- %s\n--\n", gVME->GetName());
      gSystem->Exec("more -99 sql/head_layers.sql");
      Printf("\n\n");
      Printf("INSERT INTO `layers` (`Numb`, `Tracker`, `Name`, `NChan`, `Z`, `Shift`, `Delta`, `Range`, `Direct`) VALUES");
    }

    comma = ",";
    if (opt.Contains("last")) comma = ";";
    Printf("(%3d, %2d, %10s, %3d, %8.4f, %8.4f, %8.4f, %8.4f, %2d)%s",
           fId, fTracker, Form("'%s'", GetName()), fLast + 1,
           fZ, 0.0, kDelta, kRange, fDirect, comma);
  }
}

void TLayer::SetCh(Int_t ch, Int_t nadc)
{
  if (ch >= kMaxCh) {
    Error("SetCh","limit of array is %d", kMaxCh);
    return;
  }
  fLast = ch;
  fCh[fLast] = nadc;
}

//______________________________________________________________________________

class TVirtualChamber : public TObject {
protected:
  Int_t        fNLayers;
  TLayer     **fLayers;
  Int_t        fPins[kMaxPins];
  void         Connect(Int_t mod, Int_t con, Option_t *option = "");

  void         Pattern430430() const;
  void         Pattern443300() const;
  void         Pattern33003300() const;
  void         Pattern430_3300(Int_t firstl) const;

public:
  TVirtualChamber(const TString *names);
  virtual     ~TVirtualChamber() { delete [] fLayers; }
  virtual void Print(Option_t *option = "") const;

  Int_t        GetNLayers() const { return fNLayers; }
  TLayer      *GetLayer(Int_t l) const { return (l < fNLayers) ?
      fLayers[l] : 0 ; }
  void         SetTracker(Int_t t) const { for (Int_t i = 0; i < fNLayers; i++)
      fLayers[i]->SetTracker(t); }
  void         SetTracker2(Int_t t1, Int_t t2) const {
    fLayers[0]->SetTracker(t1); fLayers[1]->SetTracker(t1);
    fLayers[2]->SetTracker(t2); fLayers[3]->SetTracker(t2); }
  void         SetZ(Double_t z) const { for (Int_t i = 0; i < fNLayers; i++)
      fLayers[i]->SetZ(z + i*kDelZ); }
  void         SetZ2(Double_t z1, Double_t z2) const {
    fLayers[0]->SetZ(z1 + 0.0); fLayers[1]->SetZ(z1 + kDelZ);
    fLayers[2]->SetZ(z2 + 0.0); fLayers[3]->SetZ(z2 + kDelZ); }
  void         SetDirect(Int_t d) const { for (Int_t i = 0; i < fNLayers; i++)
      fLayers[i]->SetDirect(d); }
  void         SetDirect2(Int_t d1, Int_t d2) const {
    fLayers[0]->SetDirect(d1); fLayers[1]->SetDirect(d1);
    fLayers[2]->SetDirect(d2); fLayers[3]->SetDirect(d2); }

  ClassDef(TVirtualChamber, 0)
};

TVirtualChamber::TVirtualChamber(const TString *names)
{
  if (list->IsEmpty()) TLayer::ResetCounter();
  list->Add(this);

  fNLayers = 4;
  fLayers = new TLayer* [fNLayers];
  for (Int_t i = 0; i < fNLayers; i++)
    fLayers[i] = new TLayer(Form("dch_%s", names[i].Data()));
}

void TVirtualChamber::Print(Option_t *option) const
{
  TString opt = option;

  if (list->First() != this) {
    if (opt.Contains("channels")) Printf("--\n-- next cham\n--");
    else if (opt.Contains("layers")) Printf("");
  }
  for (Int_t i = 0; i < fNLayers; i++) {
    opt = option;
    if ((list->First() == this) && (i == 0)) opt += "first";
    if ((list->Last() == this) && (i == (fNLayers - 1))) opt += "last";
    fLayers[i]->Print(opt.Data());
    if ((i != (fNLayers - 1)) && opt.Contains("channels")) Printf("");
  }
}

void TVirtualChamber::Connect(Int_t mod, Int_t con, Option_t *option)
{
  for (Int_t r = 0; r < kMaxPins; r++) fPins[r] = -1;
  TVirtualModule *module = (TVirtualModule *)gVME->Modules()->At(mod);
  if (!module) {
    Error("Connect", "module: %d does not exist", mod);
    return;
  }
  if (module->InheritsFrom("TModuleTDC96")) {
    TString opt = option;
    if (!opt.Contains("even") && !opt.Contains("odd")) {
      Error("Connect", "connect only 16pins cable");
      return;
    }
  }

  module->ConnectorChannels(con, fPins, option);
}

// order of channels (all pattern): first, ..., last, GND
void TVirtualChamber::Pattern430430() const
{
  // 4 consecutive layers
  for (Int_t i =  0; i <  4; i++) GetLayer(0)->AddCh(fPins[i]);
  for (Int_t i =  4; i <  7; i++) GetLayer(1)->AddCh(fPins[i]);
  for (Int_t i =  8; i < 12; i++) GetLayer(2)->AddCh(fPins[i]);
  for (Int_t i = 12; i < 15; i++) GetLayer(3)->AddCh(fPins[i]);
}

void TVirtualChamber::Pattern443300() const
{
  // 4 consecutive layers
  for (Int_t i =  0; i <  4; i++) GetLayer(0)->AddCh(fPins[i]);
  for (Int_t i =  4; i <  8; i++) GetLayer(1)->AddCh(fPins[i]);
  for (Int_t i =  8; i < 11; i++) GetLayer(2)->AddCh(fPins[i]);
  for (Int_t i = 11; i < 14; i++) GetLayer(3)->AddCh(fPins[i]);
}

void TVirtualChamber::Pattern33003300() const
{
  // 4 consecutive layers
  for (Int_t i =  0; i <  3; i++) GetLayer(0)->AddCh(fPins[i]);
  for (Int_t i =  3; i <  6; i++) GetLayer(1)->AddCh(fPins[i]);
  for (Int_t i =  8; i < 11; i++) GetLayer(2)->AddCh(fPins[i]);
  for (Int_t i = 11; i < 14; i++) GetLayer(3)->AddCh(fPins[i]);
}

void TVirtualChamber::Pattern430_3300(Int_t firstl) const
{
  // 2 consecutive layers
  for (Int_t i =  4; i <  7; i++) GetLayer(firstl + 0)->AddCh(fPins[i]);
  for (Int_t i = 11; i < 14; i++) GetLayer(firstl + 0)->AddCh(fPins[i]);
  for (Int_t i =  0; i <  4; i++) GetLayer(firstl + 1)->AddCh(fPins[i]);
  for (Int_t i =  8; i < 11; i++) GetLayer(firstl + 1)->AddCh(fPins[i]);
}

//______________________________________________________________________________

class TSmall : public TVirtualChamber {
public:
  TSmall(const TString *lnames) : TVirtualChamber(lnames) {;}
  virtual ~TSmall() {;}
  void        SetConnect(Int_t module, Int_t connector, Option_t *option = "");

  ClassDef(TSmall, 0)
};

void TSmall::SetConnect(Int_t module, Int_t connector, Option_t *option)
{
  Connect(module, connector, option);

  TString opt = option;
  opt.ToLower();
  if (opt.Contains("onlyx")) Pattern443300();
  else Pattern430430();
}

//______________________________________________________________________________

class TBig : public TVirtualChamber {
public:
  TBig(const TString *lnames) : TVirtualChamber(lnames) {;}
  virtual ~TBig() {;}
  void        SetConnect1(Int_t module, Int_t connector, Option_t *option = "");
  void        SetConnect2(Int_t module, Int_t connector, Option_t *option = "");

  ClassDef(TBig, 0)
};

void TBig::SetConnect1(Int_t module, Int_t connector, Option_t *option)
{
  Connect(module, connector, option);

  TString opt = option;
  opt.ToLower();
  if (opt.Contains("half")) Pattern430_3300(0);
  else Pattern430430();
}

void TBig::SetConnect2(Int_t module, Int_t connector, Option_t *option)
{
  Connect(module, connector, option);

  TString opt = option;
  opt.ToLower();
  if (opt.Contains("half")) Pattern430_3300(2);
  else Pattern33003300();
}
