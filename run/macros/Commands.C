{
  TFile* f1 = new TFile("lem4_5000.root");
  gROOT->ProcessLine(".L NewAnalysis.C");
  NewSpec t ;
  t.Coincidence(1,2);
}
