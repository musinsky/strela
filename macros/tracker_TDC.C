// Author: Jan Musinsky
// 07/12/2010

/*
  .x Strela.C
  .L macros/tracker_TDC.C
  TStrawCham::Tracking(0)
  gStrela->StrawCham()->SetTubesTimes(0, 0, 9000)
  tracker_TDC(t2)
  tra->Draw("T(21)>>(700,0,7000)", "Ch(21)")
  tra->Draw("T(21)", "Ch(21)&&GetNHits()==4", "same")
  tra->Draw("T_M(21, 0)>>(700,0,7000)", "Ch_M(21, 0)")
  tra->Draw("T_M(21, 0)", "Ch_M(21, 1)", "same")
  TCut cut1 = "Ch(18)&&Ch(26)&&Ch(21)"
  TCut cut2 = "Ch(18)&&Ch(26)&&Ch(21)&&GetNHits()==4"
  tra->Draw("(T(18)+T(26))/2.+T(21)>>(600,4000,7000)", cut2)
  TCut cut3 = "Ch(18)&&Ch(26)&&Ch(21)&&Ch(29)&&GetNHits()==4"
  tra->Draw("(T(18)+T(26)+T(21)+T(29))/2.>>(600,4000,7000)", cut3)
*/

void tracker_TDC(TStrawTracker *tracker = 0, Bool_t ext0 = kTRUE)
{
  if (!tracker) {
    Printf("Must specify tracker");
    return;
  }
  gROOT->LoadMacro("macros/tracker_TDC.cxx+");

  const char *fname = "tracker_TDC.root";
  TFile *file = (TFile *)gROOT->GetListOfFiles()->FindObject(fname);
  if (file) delete file;
  file = new TFile(fname, "RECREATE");
  TTree *tree = new TTree("tra", "simple TDC of tracker");
  tree->SetAutoSave(1000000000); // autosave when 1 Gbyte written
  TEvent *event = new TEvent();
  tree->Branch("Event", "TEvent", &event);

  Int_t t;
  for (Int_t i = 0; i < gStrela->GetEntries(); i++) {
    event->Clear();
    gStrela->AnalyzeEntry(i);
    for (Int_t j = 0; j < tracker->GetNHits(); j++) {
      if (ext0) t = tracker->GetTubeHit(j)->TExT0(tracker->T(j));
      else      t = tracker->T(j) - TStrawTube::GetBaseT0();
      event->AddHit(gVME->SearchChannel(tracker->GetTubeHit(j)->GetNadc()), t);
    }
    tree->Fill();
  }

  tree->Write();
  tree->Print("toponly");
  delete file;
  file = new TFile(fname, "READ");
}
