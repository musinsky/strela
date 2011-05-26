// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    16 May 2008

#include <stdlib.h>

#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>

#include "TWireCham.h"
#include "TPommeEvent.h"
#include "TStrela.h"
#include "TWirePoint.h"

const Int_t kMaxWrongWire = 10;

const Double_t kWireGap = 0.2; // [cm]
const Int_t    kMaxWireWidth = 2;

ClassImp(TWireCham)

//______________________________________________________________________________
TWireCham::TWireCham()
{
  //  Info("TWireCham", "Default Constructor");
  fNPlanes     = 0;
  fNameWP      = 0;
  fBeginWP     = 0;
  fEndWP       = 0;
  fDirectWP    = 0;
  fNWrongWires = 0;
  fWrongWire   = 0;
}
//______________________________________________________________________________
TWireCham::TWireCham(const char *name, const char *title) : TStrelaBase(name, title)
{
  //  Info("TWireCham", "Normal Constructor");
  fNPlanes     = 0;
  fNameWP      = 0;
  fBeginWP     = 0;
  fEndWP       = 0;
  fDirectWP    = 0;
  fNWrongWires = 0;
  fWrongWire   = 0;
  fContainer   = new TClonesArray("TWirePoint", 10);
  fBranchName  = "points";
}
//______________________________________________________________________________
TWireCham::~TWireCham()
{
  Info("~TWireCham", "Destructor");
  DeleteArrays();
}
//______________________________________________________________________________
void TWireCham::DeleteArrays()
{
  fNPlanes  = 0;
  delete [] fNameWP;
  delete [] fBeginWP;
  delete [] fEndWP;
  delete [] fDirectWP;
  DeleteWrongWires();
}
//______________________________________________________________________________
void TWireCham::SetWrongWire(Int_t ew)
{
  if (fNWrongWires == 0) fWrongWire = new Int_t[kMaxWrongWire];
  if (fNWrongWires >= kMaxWrongWire) {
    Error("SetWrongWire", "to many wrong wires, limit is %d", kMaxWrongWire);
    return;
  }
  fWrongWire[fNWrongWires++] = ew;
  PrintWrongWire();
}
//______________________________________________________________________________
void TWireCham::PrintWrongWire() const
{
  Printf("Number of all wrong wires = %d", fNWrongWires);
  for (Int_t iw = 0; iw < fNWrongWires; iw++)
    Printf("%2d wrong wire = %03d", iw, fWrongWire[iw]);
}
//______________________________________________________________________________
void TWireCham::DeleteWrongWires()
{
  fNWrongWires = 0;
  delete [] fWrongWire;
}
//______________________________________________________________________________
Bool_t TWireCham::ReadSQL(TSQLServer *ser)
{
  DeleteArrays();
  Clear();
  const char *table = "detectors";
  const char *pattern = "small%";
  TSQLResult *res = ser->Query(Form("SELECT `Name`, `Begin`, `End`, `Direct` FROM `%s` WHERE `Name` LIKE '%s'", table, pattern));
  TSQLRow *row;

  if (res) {
    fNPlanes = res->GetRowCount();
    if (fNPlanes > 0) {
      fNameWP   = new TString[fNPlanes];
      fBeginWP  = new Int_t[fNPlanes];
      fEndWP    = new Int_t[fNPlanes];
      fDirectWP = new Int_t[fNPlanes];
      for (Int_t i = 0; i < fNPlanes; i++) {
        row = res->Next();
        fNameWP[i]   = row->GetField(0);
        fBeginWP[i]  = atoi(row->GetField(1));
        fEndWP[i]    = atoi(row->GetField(2));
        fDirectWP[i] = atoi(row->GetField(3));
        delete row;
      }
    }
    delete res;
  }
  delete ser;

  if (fNPlanes == 0) {
    Info("ReadSQL", "working without any planes");
    return kFALSE;
  }
  return kTRUE;
}
//______________________________________________________________________________
void TWireCham::AnalyzeEntry()
{
  Clear();
  TPommeEvent *pommeEvent = gStrela->PommeEvent();
  TChWire *wireHit;

  Int_t wire, width, plane;
  Double32_t point;

  for (Int_t iw = 0; iw < pommeEvent->GetNumOfWires(); iw++) {
    wireHit = pommeEvent->GetWireHit(iw);
    wire    = wireHit->GetWire();
    plane   = FindPlane(wire);
    if (plane < 0) continue;
    width   = wireHit->GetWidth();
    if (width > kMaxWireWidth) continue;
    point   = kWireGap*(wire - (fEndWP[plane] + fBeginWP[plane])/2);
    point  += kWireGap/2.*width;
    point  *= fDirectWP[plane];
    AddPoint(plane, point);
  }
}
//______________________________________________________________________________
TWirePoint *TWireCham::AddPoint(Int_t pl, Double32_t po)
{
  TClonesArray &points = *Containers();
  Int_t ip = points.GetLast() + 1;
  return new(points[ip]) TWirePoint(pl, po);
}
//______________________________________________________________________________
Int_t TWireCham::FindPlane(Int_t w) const
{
  // for large array will be better TMath::BinarySearch
  for (Int_t ie = 0; ie < fNWrongWires; ie++)
    if (fWrongWire[ie] == w) return -1;

  for (Int_t ip = 0; ip < fNPlanes; ip++)
    if ((w >= fBeginWP[ip]) && (w < fEndWP[ip])) return ip;

  Warning("FindPlane", "wire %03d is outside of all planes", w);
  return -2;
}
