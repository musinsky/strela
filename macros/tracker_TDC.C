// Author: Jan Musinsky
// 11/11/2010

/*
  .x Strela.C
  .L macros/tracker_TDC.C
  tracker_TDC(t2)
  tracker_TDC(t2, 99)
  tra->Draw("T(21)>>(700,0,7000)","Ch(21)&&GetNHits()==4")
  tra->Draw("(T(18)+T(26))/2.+T(21)>>(600,4000,7000)","Ch(18)&&Ch(26)&&Ch(21)")
*/

void tracker_TDC(TStrawTracker *tracker = 0, Int_t max_hits = -1)
{
  if (!tracker) {
    Printf("Must specify tracker");
    return;
  }
  gROOT->LoadMacro("macros/tracker_TDC.cxx+");
  Int_t prev_t = TStrawCham::GetTracking();
  TStrawCham::Tracking(0);
  gStrela->StrawCham()->SetTubesTime(0, -9999, 9999);

  const char *fname = "tracker_TDC.root";
  TFile *file = (TFile *)gROOT->GetListOfFiles()->FindObject(fname);
  if (file) delete file;
  file = new TFile(fname, "RECREATE");
  TTree *tree = new TTree("tra", "simple TDC of tracker");
  tree->SetAutoSave(1000000000); // autosave when 1 Gbyte written
  TEvent *event = new TEvent();
  tree->Branch("Event", "TEvent", &event);

  if (max_hits == -1) max_hits = tracker->Layers()->GetSize();
  for (Int_t i = 0; i < gStrela->GetEntries(); i++) {
    gStrela->AnalyzeEntry(i);
    event->Clear();
    if (tracker->GetNHits() > max_hits) {
      tree->Fill();
      continue;
    }
    for (Int_t j = 0; j < tracker->GetNHits(); j++)
      event->AddHit(gVME->SearchChannel(tracker->GetTubeHit(j)->GetNadc()),
                    tracker->T(j) + tracker->GetTubeHit(j)->GetT0());
    tree->Fill();
  }
  tree->Write();
  tree->Print("toponly");
  delete file;
  file = new TFile(fname, "READ");

  // previous values
  TStrawCham::Tracking(prev_t);
  gStrela->StrawCham()->SetTubesTime();
}
