// To run: root -b LoopFiles.C

{
  TString fileDir,fileName,file;
  Int_t fileNumber,L1,L3;
  
  gROOT->ProcessLine(".L BeamEnv.C");
BeamSlices("data/musr_142180.root",851,904);
}
