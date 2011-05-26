// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    24 Oct 2007

#include "TWirePoint.h"

ClassImp(TWirePoint)

//______________________________________________________________________________
TWirePoint::TWirePoint(Int_t plane, Double32_t point)
{
  fPlane = plane;
  fPoint = point;
}
