// To run: root -b LoopFiles.C

{
  TString fileDir,fileName,file;
  Int_t fileNumber,L1,L3;
  
  gROOT->ProcessLine(".L BeamEnv.C");

  fileDir = "data/";
  //printf("Run \t Asy \t dAsy \t Chi2 \t N\n");
 
  fileNumber=0;
  for (L3=0; L3<=10 ; L3++) {
    for (L1=0; L1<=10 ; L1++) {
      for (CR=0; CR<=16; CR++) {
	fileNumber++;
	file.Form("musr_115%02d.root",fileNumber);
	fileName= fileDir + file;
	//      printf("File is : %s\n", (char*) fileName);
	NatSlice(fileName,901);
      }
      printf("\n");
    }
  }
}

