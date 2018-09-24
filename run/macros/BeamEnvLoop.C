// To run: root -b LoopFiles.C

{
  TString fileDir,fileName,file;
  Int_t fileNumber;
  
  gStyle->SetPalette(1,0);
  TCanvas* c1= new TCanvas("c1","canvas 1");
  

  gROOT->ProcessLine(".L BeamEnv.C");
  //  gROOT->ProcessLine(".L sr_tr.C");

  fileDir = "./data/";
  printf("Run \t Z \t Xrms \t Yrms \t Xavg \t Yavg \t N\n");
 
  for (fileNumber=52421; fileNumber<=52424; fileNumber++) 
    {
      file.Form("musr_%04d.root",fileNumber);
      fileName= fileDir + file;
      EnvatZ(fileNumber,fileName,c1);
    }
}

