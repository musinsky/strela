// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    12 Oct 2011

#ifndef STRELA_TWirePoint
#define STRELA_TWirePoint

#ifndef ROOT_TObject
#include <TObject.h>
#endif

class TWirePoint : public TObject {

private:
  Int_t         fPlane; //  Plane
  Double32_t    fPoint; //  Point

public:
  TWirePoint() : fPlane(0), fPoint(0) {;}
  TWirePoint(Int_t plane, Double32_t point);
  virtual      ~TWirePoint() {;}

  Int_t         GetPlane() const { return fPlane; }
  Double32_t    GetPoint() const { return fPoint; }

  ClassDef(TWirePoint, 1) // WirePoint class
};

#endif
