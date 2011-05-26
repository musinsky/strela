// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    24 Oct 2007

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
  TWirePoint() {;}
  TWirePoint(Int_t plane, Double32_t point);
  virtual      ~TWirePoint() {;}

  Int_t         GetPlane() const { return fPlane; }
  Double32_t    GetPoint() const { return fPoint; }

  ClassDef(TWirePoint, 1) // WirePoint class
};

#endif
