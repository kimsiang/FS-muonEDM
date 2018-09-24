// To run: root -b LoopFiles.C

{
  TString fileDir,fileName,file;
  Int_t fileNumber,CR;
  
  gROOT->ProcessLine(".L BeamEnv.C");

  fileDir = "data/";
  //printf("Run \t Asy \t dAsy \t Chi2 \t N\n");
 
  fileNumber=0;
  for (CR=0; CR<=16 ; CR++) {
      fileNumber++;
      file.Form("musr_101%02d.root",fileNumber);
      fileName= fileDir + file;
      //      printf("File is : %s\n", (char*) fileName);
      //      NatSlice(fileName,901);
      printf("%d\t",CR);
      BeamSlice(fileName,901);
      //      printf("\n");
  }
}

