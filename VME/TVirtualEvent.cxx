// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    19 Mar 2015

#include "TVirtualEvent.h"
#include "TVirtualHit.h"
#include "TVME.h"

ClassImp(TVirtualEvent)

//______________________________________________________________________________
TVirtualEvent::TVirtualEvent()
: TObject(),
  fNHits(0),
  fHits(0)
{
  // Default constructor
  if (!gVME) Warning("TVirtualEvent", "gVME not initialized");
}
//______________________________________________________________________________
TVirtualEvent::~TVirtualEvent()
{
  //  Info("~TVirtualEvent", "Destructor");
  if (fHits) fHits->Delete();
  SafeDelete(fHits);
}
//______________________________________________________________________________
void TVirtualEvent::Clear(Option_t *option)
{
  fNHits = 0;
  fHits->Clear(option);
}
//______________________________________________________________________________
void TVirtualEvent::IndexHitChanMulti()
{
  // indexing all hits, must be call (but only once) for each given event

  fIdxHitChanLast.Reset();
  for (Int_t multi = 0; multi < kMaxMulti; multi++)
    fIdxHitChanMulti[multi].Reset();

  for (Int_t i = 0; i < fNHits; i++) {
    Int_t ch = GetHit(i)->GetChannel();
    for (Int_t multi = 0; multi < kMaxMulti; multi++)
      if (fIdxHitChanMulti[multi].At(ch) == 0) {
        fIdxHitChanMulti[multi].AddAt(i+1, ch);
        break;
      }
    fIdxHitChanLast.AddAt(i+1, ch); // always last hit
  }

  SetBit(kIdxEvent, kTRUE); // only once per event
}
//______________________________________________________________________________
Int_t TVirtualEvent::GetIndexHit(Int_t ch, Int_t multi)
{
  // trick how call (but only once) IndexHitChanMulti() for each given event
  // bit kIdxEvent from stored event (from root file) is always kFALSE
  // works wih TTree::Draw (Scan or Query even thought call more than once)
  // don't use TVirtualEvent::Class()->IgnoreTObjectStreamer(kTRUE);

  if (!TestBit(kIdxEvent)) IndexHitChanMulti();

  if ((multi < 0) || (multi >= kMaxMulti))
    return (fIdxHitChanLast.At(ch)-1);
  else
    return (fIdxHitChanMulti[multi].At(ch)-1);
}
//______________________________________________________________________________
Int_t TVirtualEvent::Multi(Int_t ch)
{
  Int_t lastIdx = GetIndexHit(ch, -1);
  if (lastIdx == -1) return 0;

  for (Int_t multi = 0; multi < kMaxMulti; multi++)
    if ((fIdxHitChanMulti[multi].At(ch)-1) == lastIdx) return (multi+1);

  return (kMaxMulti+1);
}
