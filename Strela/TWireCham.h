// -*- mode: c++ -*-
// Author: Jan Musinsky <mailto:musinsky@gmail.com>
// @(#) 24 Oct 2007

#ifndef STRELA_TWireCham
#define STRELA_TWireCham

#ifndef STRELA_TStrelaBase
#include "TStrelaBase.h"
#endif

class TWirePoint;

class TWireCham : public TStrelaBase {

private:
  Int_t         fNPlanes;     //  number of planes
  TString      *fNameWP;      //[fNPlanes] name of plane
  Int_t        *fBeginWP;     //[fNPlanes] first wire(channel) of wire plane
  Int_t        *fEndWP;       //[fNPlanes] last wire(channel) of wire plane
  Int_t        *fDirectWP;    //[fNPlanes] direction of wire plane
  Int_t         fNWrongWires; //  number of error wires
  Int_t        *fWrongWire;   //[fNWrongWires] array of error wires

public:
  TWireCham();
  TWireCham(const char *name, const char *title);
  virtual      ~TWireCham();

  Int_t         GetNPlanes() const { return fNPlanes; }
  TString      *GetNameWP() const { return fNameWP; }
  Int_t        *GetBeginWP() const { return fBeginWP; }
  Int_t        *GetEndWP() const { return fEndWP; }
  Int_t        *GetDirectWP() const { return fDirectWP; }
  Int_t         GetNWrongWires() const { return fNWrongWires; }
  Int_t        *GetWrongWire() const { return fWrongWire; }
  Int_t         GetNumOfPoints() const { return GetNumOfContainers(); }
  TWirePoint   *GetWirePoint(Int_t ip) const { return
      (TWirePoint *)Container(ip); }

  virtual Bool_t ReadSQL(TSQLServer *ser);
  virtual void   AnalyzeEntry();

  void          DeleteArrays();
  void          SetWrongWire(Int_t ew);
  void          PrintWrongWire() const;
  void          DeleteWrongWires();
  TWirePoint   *AddPoint(Int_t pl, Double32_t po);
  Int_t         FindPlane(Int_t w) const;

  ClassDef(TWireCham, 1) // WireCham class
};

#endif
