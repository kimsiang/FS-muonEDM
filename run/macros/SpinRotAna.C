char popis[100];

void SpinRotAna(){
  cout<<"Syntax:  root[]> SpinRotAna.C(\"20021\")"<<endl;
  exit(0);
}

void SpinRotAna(char* runID) {
  cout<<"runID="<<runID<<endl;
  char inputFileName[1000]="data/musr_"; strcat(inputFileName,runID); strcat(inputFileName,".root");
  cout<<"Opening file "<<inputFileName<<endl;
  TFile* f1=new TFile(inputFileName);
  //  TFile* f1=new TFile("data/musr_20021.root");
  // PlotOneSaveVolume();

  strcpy(popis,"undefined");
  if (strcmp(runID,"20053")==0) {strcpy(popis,"#splitline{Run 20053: E_{const}=2.43 kV/mm; v04=0.03 T}{pie3\_HiField\_d05\_x30\_z-15111}");}
  if (strcmp(runID,"20063")==0) {strcpy(popis,"#splitline{Run 20063: e01=2.55 kV/mm; v04=0.03 T}{pie3\_HiField\_d05\_x30\_z-15111}");}
  if (strcmp(runID,"20064")==0) {strcpy(popis,"#splitline{Run 20064: e01=2.57 kV/mm; v04=0.03 T}{pie3\_HiField\_d05\_x30\_z-15111}");}
  if (strcmp(runID,"20073")==0) {strcpy(popis,"#splitline{Run 20073: e01=2.90 kV/mm; v04=0.0339 T}{pie3\_HiField\_d05\_x30\_z-15111}");}
  if (strcmp(runID,"20074")==0) {strcpy(popis,"#splitline{Run 20074: e01=2.95 kV/mm; v04=0.0339 T}{pie3\_HiField\_d05\_x30\_z-15111}");}
  if (strcmp(runID,"20075")==0) {strcpy(popis,"#splitline{Run 20075: e01=3.00 kV/mm; v04=0.0339 T}{pie3\_HiField\_d05\_x30\_z-15111}");}
  if (strcmp(runID,"20076")==0) {strcpy(popis,"#splitline{Run 20076: e01=3.05 kV/mm; v04=0.0339 T}{pie3\_HiField\_d05\_x30\_z-15111}");}
  if (strcmp(runID,"20081")==0) {strcpy(popis,"#splitline{Run 20081: e01=3.00 kV/mm; v04=0.0339 T}{pie3\_HiField\_d05\_x30\_z-15111}");}
  if (strcmp(runID,"20082")==0) {strcpy(popis,"#splitline{Run 20082: e01=3.00 kV/mm; v04=0.0339 T}{pie3\_HiField\_d05\_x30\_z-15111}");}
  if (strcmp(runID,"20111")==0) {strcpy(popis,"#splitline{Run 20111: e01=3.00 kV/mm; v04=0.0339 T}{pie3\_HiField\_d05\_x30\_z-15111}");}
  if (strcmp(runID,"20116")==0) {strcpy(popis,"#splitline{Run 20116: e01=3.00 kV/mm; v04=0.0339 T}{pie3\_HiField\_d05\_x30\_z-15111}");}
  if (strcmp(runID,"20121")==0) {strcpy(popis,"#splitline{Run 20121: e01=3.00 kV/mm; b05c=0.0339 T}{pie3\_HiField\_d05\_x30\_z-15111}");}
  if (strcmp(runID,"20131")==0) {strcpy(popis,"#splitline{Run 20131: e01=3.00 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20132")==0) {strcpy(popis,"#splitline{Run 20132: e01=3.03 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20133")==0) {strcpy(popis,"#splitline{Run 20133: e01=3.06 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20134")==0) {strcpy(popis,"#splitline{Run 20134: e01=2.97 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20135")==0) {strcpy(popis,"#splitline{Run 20135: e01=2.94 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20141")==0) {strcpy(popis,"#splitline{Run 20141: e01=3.00 kV/mm; v04=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20142")==0) {strcpy(popis,"#splitline{Run 20142: e01=3.03 kV/mm; v04=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20143")==0) {strcpy(popis,"#splitline{Run 20143: e01=3.06 kV/mm; v04=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20144")==0) {strcpy(popis,"#splitline{Run 20144: e01=2.97 kV/mm; v04=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20145")==0) {strcpy(popis,"#splitline{Run 20145: e01=2.94 kV/mm; v04=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20181")==0) {strcpy(popis,"#splitline{Run 20181: e01=3.09 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20191")==0) {strcpy(popis,"#splitline{Run 20191: e01=3.09 kV/mm; v04=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20201")==0) {strcpy(popis,"#splitline{Run 20201: e01=3.09 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032, y-x}");}
  if (strcmp(runID,"20201")==0) {strcpy(popis,"#splitline{Run 20201: e01=3.09 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032, y-x}");}
  if (strcmp(runID,"20202")==0) {strcpy(popis,"#splitline{Run 20202: e01=3.09 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032, -yx}");}
  if (strcmp(runID,"20203")==0) {strcpy(popis,"#splitline{Run 20203: e01=3.09 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20211")==0) {strcpy(popis,"#splitline{Run 20211: e01=3.09 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032, y-x}");}
  if (strcmp(runID,"20212")==0) {strcpy(popis,"#splitline{Run 20212: e01=3.09 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032, -yx}");}
  if (strcmp(runID,"20213")==0) {strcpy(popis,"#splitline{Run 20213: e01=3.09 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032}");}
  if (strcmp(runID,"20221")==0) {strcpy(popis,"#splitline{Run 20221: e01=3.09 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032\_NEW, y-x}");}
  if (strcmp(runID,"20222")==0) {strcpy(popis,"#splitline{Run 20222: e01=3.09 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032\_NEW, -yx}");}
  if (strcmp(runID,"20223")==0) {strcpy(popis,"#splitline{Run 20223: e01=3.09 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032\_NEW}");}
  if (strcmp(runID,"20231")==0) {strcpy(popis,"#splitline{Run 20231: e01=3.09 kV/mm; b05c=0.0339 T}{reggiani\_Jan2010\_z-12032\_NEW}");}
  if (strcmp(runID,"20301")==0) {strcpy(popis,"#splitline{Run 20301: e01=3.09 kV/mm; b05c=0.0339 T}{like 20181 with Geant4.9.3}");}
  if (strcmp(runID,"20302")==0) {strcpy(popis,"#splitline{Run 20302: e01=3.09 kV/mm; b05c=0.0339 T}{like 20181}");}
  if (strcmp(runID,"20303")==0) {strcpy(popis,"#splitline{Run 20303: e01=3.09 kV/mm; b05c=0.0339 T}{like 20181 with Geant4.9.3 new}");}
  if (strcmp(runID,"20311")==0) {strcpy(popis,"#splitline{Run 20311: e01=3.09 kV/mm; b05c=0.0339 T}{Feb Turtle,  Geant4.9.3}");}
  if (strcmp(runID,"20312")==0) {strcpy(popis,"#splitline{Run 20312: e01=3.00 kV/mm; b05c=0.0339 T}{Feb Turtle,  Geant4.9.3}");}
  if (strcmp(runID,"20313")==0) {strcpy(popis,"#splitline{Run 20313: e01=3.03 kV/mm; b05c=0.0339 T}{Feb Turtle,  Geant4.9.3}");}
  if (strcmp(runID,"20314")==0) {strcpy(popis,"#splitline{Run 20314: e01=3.06 kV/mm; b05c=0.0339 T}{Feb Turtle,  Geant4.9.3}");}
  if (strcmp(runID,"20315")==0) {strcpy(popis,"#splitline{Run 20315: e01=3.12 kV/mm; b05c=0.0339 T}{Feb Turtle,  Geant4.9.3}");}
  if (strcmp(runID,"20316")==0) {strcpy(popis,"#splitline{Run 20316: e01=3.15 kV/mm; b05c=0.0339 T}{Feb Turtle,  Geant4.9.3}");}
  if (strcmp(runID,"20321")==0) {strcpy(popis,"#splitline{Run 20321: e01=3.03 kV/mm; b05c=0.0339 T}{Feb Turtle,  Geant4.9.3}");}
  if (strcmp(runID,"20331")==0) {strcpy(popis,"#splitline{Run 20331: E_{unif}=3.03 kV/mm; B_{unif}=0.0392 T}{Feb Turtle,  Geant4.9.3}");}
  if (strcmp(runID,"20332")==0) {strcpy(popis,"#splitline{Run 20332: E_{unif}=3.00 kV/mm; B_{unif}=0.0392 T}{Feb Turtle,  Geant4.9.3}");}
  if (strcmp(runID,"20333")==0) {strcpy(popis,"#splitline{Run 20333: E_{unif}=3.06 kV/mm; B_{unif}=0.0392 T}{Feb Turtle,  Geant4.9.3}");}
  if (strcmp(runID,"20334")==0) {strcpy(popis,"#splitline{Run 20334: E_{unif}=3.09 kV/mm; B_{unif}=0.0392 T}{Feb Turtle,  Geant4.9.3}");}
  if (strcmp(runID,"20335")==0) {strcpy(popis,"#splitline{Run 20335: E_{unif}=2.97 kV/mm; B_{unif}=0.0392 T}{Feb Turtle,  Geant4.9.3}");}
  if (strcmp(runID,"20351")==0) {strcpy(popis,"#splitline{Run 20351: E_{unif}=3.08544 kV/mm; B_{unif}=0.041 T}{Feb Turtle}");}
  if (strcmp(runID,"20361")==0) {strcpy(popis,"#splitline{Run 20361: E_{unif}=3.08544 kV/mm; B_{unif}=0.041 T}{p=27.4 MeV/c, x=y=0}");}
  if (strcmp(runID,"20362")==0) {strcpy(popis,"#splitline{Run 20362: E_{unif}=3.08544 kV/mm; B_{unif}=0.041 T}{p=27.4+/-0.81 MeV/c, x=y=0}");}
  if (strcmp(runID,"20363")==0) {strcpy(popis,"#splitline{Run 20363: E_{unif}=3.08544 kV/mm; B_{unif}=0.041 T}{p=27.4 MeV/c, x,y=Gaussian}");}
  if (strcmp(runID,"20371")==0) {strcpy(popis,"#splitline{Run 20371: e01=3.03 kV/mm; b05c=0.0339 T}{Feb Turtle,  Geant4.9.3}");}
  if (strcmp(runID,"20381")==0) {strcpy(popis,"#splitline{Run 20381: E_{unif}=3.08544 kV/mm; B_{unif}=0.041 T}{Feb Turtle, #mu does not decay}");}
  if (strcmp(runID,"20391")==0) {strcpy(popis,"#splitline{Run 20391: E_{unif}=3.08544 kV/mm; B_{unif}=0.041 T}{p=27.4 MeV/c, x=y=0, #mu does not decay}");}
  if (strcmp(runID,"20392")==0) {strcpy(popis,"#splitline{Run 20392: E_{unif}=3.08544 kV/mm; B_{unif}=0.041 T}{p=27.4+/-0.81 MeV/c, x=y=0, #mu does not decay}");}
  if (strcmp(runID,"20393")==0) {strcpy(popis,"#splitline{Run 20393: E_{unif}=3.08544 kV/mm; B_{unif}=0.041 T}{p=27.4 MeV/c, x,y=Gaussian, #mu does not decay}");}


  TCanvas* cR1IN = PlotOneSaveVolume   ("R1IN", "save_detID==1010&&save_particleID==-13");
  TCanvas* cR1IN2= PlotOneSaveVolumeTwo("R1IN2","save_detID==1010&&save_particleID==-13");
  char outputFileName_cR1IN[1000];  sprintf(outputFileName_cR1IN,"pict/SpinRotAna_R1IN_%s.ps",runID);
  cR1IN->Print(outputFileName_cR1IN);
  char outputFileName_cR1IN2[1000];  sprintf(outputFileName_cR1IN2,"pict/SpinRotAna_R1IN_two_%s.ps",runID);
  cR1IN2->Print(outputFileName_cR1IN2);

  TCanvas* cR1OU = PlotOneSaveVolume   ("R1OU", "save_detID==1020&&save_particleID==-13");
  TCanvas* cR1OU2= PlotOneSaveVolumeTwo("R1OU2","save_detID==1020&&save_particleID==-13");
  char outputFileName_cR1OU[1000];  sprintf(outputFileName_cR1OU,"pict/SpinRotAna_R1OU_%s.ps",runID);
  cR1OU->Print(outputFileName_cR1OU);
  char outputFileName_cR1OU2[1000];  sprintf(outputFileName_cR1OU2,"pict/SpinRotAna_R1OU_two_%s.ps",runID);
  cR1OU2->Print(outputFileName_cR1OU2);

  TCanvas* cR2IN = PlotOneSaveVolume   ("R2IN", "save_detID==1050&&save_particleID==-13");
  TCanvas* cR2IN2= PlotOneSaveVolumeTwo("R2IN2","save_detID==1050&&save_particleID==-13");
  char outputFileName_cR2IN[1000];  sprintf(outputFileName_cR2IN,"pict/SpinRotAna_R2IN_%s.ps",runID);
  cR2IN->Print(outputFileName_cR2IN);
  char outputFileName_cR2IN2[1000];  sprintf(outputFileName_cR2IN2,"pict/SpinRotAna_R2IN_two_%s.ps",runID);
  cR2IN2->Print(outputFileName_cR2IN2);

  TCanvas* cR2OU = PlotOneSaveVolume   ("R2OU", "save_detID==1060&&save_particleID==-13");
  TCanvas* cR2OU2= PlotOneSaveVolumeTwo("R2OU2","save_detID==1060&&save_particleID==-13");
  char outputFileName_cR2OU[1000];  sprintf(outputFileName_cR2OU,"pict/SpinRotAna_R2OU_%s.ps",runID);
  cR2OU->Print(outputFileName_cR2OU);
  char outputFileName_cR2OU2[1000];  sprintf(outputFileName_cR2OU2,"pict/SpinRotAna_R2OU_two_%s.ps",runID);
  cR2OU2->Print(outputFileName_cR2OU2);
  


  TCanvas* cEnvelope = new TCanvas("cEnvelope","cEnvelope");
  //  TH1D* hisEnvelopeX = new TH1D("hisEnvelopeX","Beam envelope; z (cm); 2*RMS of x and -y (cm)",1200,-1700,-500);
  //  TH1D* hisEnvelopeY = new TH1D("hisEnvelopeY","Beam envelope; z (cm); 2*RMS of x and -y (cm)",1200,-1700,-500);
  TH1D* hisEnvelopeX = new TH1D("hisEnvelopeX","Beam envelope; z (cm); #pm 2*RMS of x and y (cm)",1200,-1400,-200);
  TH1D* hisEnvelopeY = new TH1D("hisEnvelopeY","Beam envelope; z (cm); #pm 2*RMS of x and y (cm)",1200,-1400,-200);
  Double_t fRunID=atof(runID);
  std::cout<<"fRunID="<<fRunID<<std::endl;
  if (fRunID<20130) {
    FillEnvelope(hisEnvelopeX,-1511.15,"save_x/10>>hh","save_detID==1010&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-1241.0,"save_x/10>>hh","save_detID==1020&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-1200,"save_x/10>>hh","save_detID==1025&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-1160,"save_x/10>>hh","save_detID==1028&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-1146.6,"save_x/10>>hh","save_detID==1030&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-1086.6,"save_x/10>>hh","save_detID==1033&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-1026.6,"save_x/10>>hh","save_detID==1036&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-972.15,"save_x/10>>hh","save_detID==1040&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-962.15,"save_x/10>>hh","save_detID==1041&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-952.15,"save_x/10>>hh","save_detID==1042&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-942.15,"save_x/10>>hh","save_detID==1043&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-932.15,"save_x/10>>hh","save_detID==1044&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-922.15,"save_x/10>>hh","save_detID==1045&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-912.15,"save_x/10>>hh","save_detID==1046&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-902.15,"save_x/10>>hh","save_detID==1047&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-892.15,"save_x/10>>hh","save_detID==1048&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-882.15,"save_x/10>>hh","save_detID==1049&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-872.15,"save_x/10>>hh","save_detID==1050&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-602.0 ,"save_x/10>>hh","save_detID==1060&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-592.0 ,"save_x/10>>hh","save_detID==1061&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-582.0 ,"save_x/10>>hh","save_detID==1062&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-572.0 ,"save_x/10>>hh","save_detID==1063&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-562.0 ,"save_x/10>>hh","save_detID==1064&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-552.0 ,"save_x/10>>hh","save_detID==1065&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-542.0 ,"save_x/10>>hh","save_detID==1066&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-532.0 ,"save_x/10>>hh","save_detID==1067&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-522.0 ,"save_x/10>>hh","save_detID==1068&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-512.0 ,"save_x/10>>hh","save_detID==1069&&save_particleID==-13");

    FillEnvelope(hisEnvelopeY,-1511.15,"save_y/10>>hh","save_detID==1010&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-1241.0,"save_y/10>>hh","save_detID==1020&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-1200,"save_y/10>>hh","save_detID==1025&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-1160,"save_y/10>>hh","save_detID==1028&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-1146.6,"save_y/10>>hh","save_detID==1030&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-1086.6,"save_y/10>>hh","save_detID==1033&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-1026.6,"save_y/10>>hh","save_detID==1036&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-972.15,"save_y/10>>hh","save_detID==1040&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-962.15,"save_y/10>>hh","save_detID==1041&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-952.15,"save_y/10>>hh","save_detID==1042&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-942.15,"save_y/10>>hh","save_detID==1043&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-932.15,"save_y/10>>hh","save_detID==1044&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-922.15,"save_y/10>>hh","save_detID==1045&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-912.15,"save_y/10>>hh","save_detID==1046&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-902.15,"save_y/10>>hh","save_detID==1047&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-892.15,"save_y/10>>hh","save_detID==1048&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-882.15,"save_y/10>>hh","save_detID==1049&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-872.15,"save_y/10>>hh","save_detID==1050&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-602.0 ,"save_y/10>>hh","save_detID==1060&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-592.0 ,"save_y/10>>hh","save_detID==1061&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-582.0 ,"save_y/10>>hh","save_detID==1062&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-572.0 ,"save_y/10>>hh","save_detID==1063&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-562.0 ,"save_y/10>>hh","save_detID==1064&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-552.0 ,"save_y/10>>hh","save_detID==1065&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-542.0 ,"save_y/10>>hh","save_detID==1066&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-532.0 ,"save_y/10>>hh","save_detID==1067&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-522.0 ,"save_y/10>>hh","save_detID==1068&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-512.0 ,"save_y/10>>hh","save_detID==1069&&save_particleID==-13");
  }
  else {
    FillEnvelope(hisEnvelopeX,-1183.3,"save_x/10>>hh","save_detID==1010&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-913.1,"save_x/10>>hh","save_detID==1020&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-903.1,"save_x/10>>hh","save_detID==1025&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-893.1,"save_x/10>>hh","save_detID==1026&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-880.0,"save_x/10>>hh","save_detID==1028&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-870.0,"save_x/10>>hh","save_detID==1030&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-820.0,"save_x/10>>hh","save_detID==1033&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-759.0,"save_x/10>>hh","save_detID==1036&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-710.0,"save_x/10>>hh","save_detID==1045&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-707.0,"save_x/10>>hh","save_detID==1046&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-697.0,"save_x/10>>hh","save_detID==1047&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-687.0,"save_x/10>>hh","save_detID==1048&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-677.0,"save_x/10>>hh","save_detID==1049&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-667.0,"save_x/10>>hh","save_detID==1050&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-395.0,"save_x/10>>hh","save_detID==1060&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-385.0,"save_x/10>>hh","save_detID==1061&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-375.0,"save_x/10>>hh","save_detID==1062&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-365.0,"save_x/10>>hh","save_detID==1063&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-355.0,"save_x/10>>hh","save_detID==1064&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-345.0,"save_x/10>>hh","save_detID==1065&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-335.0,"save_x/10>>hh","save_detID==1066&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-325.0,"save_x/10>>hh","save_detID==1067&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-315.0,"save_x/10>>hh","save_detID==1068&&save_particleID==-13");
    FillEnvelope(hisEnvelopeX,-305.0,"save_x/10>>hh","save_detID==1069&&save_particleID==-13");

    FillEnvelope(hisEnvelopeY,-1183.3,"save_y/10>>hh","save_detID==1010&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-913.1,"save_y/10>>hh","save_detID==1020&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-903.1,"save_y/10>>hh","save_detID==1025&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-893.1,"save_y/10>>hh","save_detID==1026&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-880.0,"save_y/10>>hh","save_detID==1028&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-870.0,"save_y/10>>hh","save_detID==1030&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-820.0,"save_y/10>>hh","save_detID==1033&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-759.0,"save_y/10>>hh","save_detID==1036&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-710.0,"save_y/10>>hh","save_detID==1045&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-707.0,"save_y/10>>hh","save_detID==1046&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-697.0,"save_y/10>>hh","save_detID==1047&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-687.0,"save_y/10>>hh","save_detID==1048&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-677.0,"save_y/10>>hh","save_detID==1049&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-667.0,"save_y/10>>hh","save_detID==1050&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-395.0,"save_y/10>>hh","save_detID==1060&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-385.0,"save_y/10>>hh","save_detID==1061&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-375.0,"save_y/10>>hh","save_detID==1062&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-365.0,"save_y/10>>hh","save_detID==1063&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-355.0,"save_y/10>>hh","save_detID==1064&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-345.0,"save_y/10>>hh","save_detID==1065&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-335.0,"save_y/10>>hh","save_detID==1066&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-325.0,"save_y/10>>hh","save_detID==1067&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-315.0,"save_y/10>>hh","save_detID==1068&&save_particleID==-13");
    FillEnvelope(hisEnvelopeY,-305.0,"save_y/10>>hh","save_detID==1069&&save_particleID==-13");
  }


  hisEnvelopeX->SetMaximum(19);
  hisEnvelopeX->SetMinimum(-19);
  hisEnvelopeX->SetMarkerStyle(20);
  hisEnvelopeX->Draw("P");
  hisEnvelopeY->SetMarkerStyle(20);
  hisEnvelopeY->Draw("Psame");
  Vysvetlivka(popis,0.15,0.14,90,0.025);
  char outputFileName_cR2OU[1000];  sprintf(outputFileName_cR2OU,"pict/SpinRotAna_Envelope_%s.ps",runID);
  gPad->SetGrid();
  cEnvelope->Print(outputFileName_cR2OU);

}

TCanvas* PlotOneSaveVolume(char* name, char* condition) {
  cout<<"PlotOneSaveVolume called"<<endl;
  char cName[100]; strcpy(cName,"c"); strcat(cName,name);
  TCanvas* c1= new TCanvas(cName,cName,700,900);
  c1->Divide(2,3);

  c1->cd(1);
  char hNameX[200]; strcpy(hNameX,"h"); strcat(hNameX,name); strcat(hNameX,"x");
  TH1D* hisX = new TH1D(hNameX,hNameX,100,-25,25);
  hisX->GetXaxis()->SetTitle("x (cm)");
  char whatToPlotX[200]; strcpy(whatToPlotX,"save_x/10>>"); strcat(whatToPlotX,hNameX);
  t1->Draw(whatToPlotX,condition);
  if (strcmp(popis,"undefined")) {
    Vysvetlivka(popis,0.2,0.14,90);
  }
  char chTransmission[200];
  Double_t transmission = (hisX->GetEntries())/(t1->GetEntries())*100;
  sprintf(chTransmission,"Transmission: %.1f %%",transmission);
  Vysvetlivka(chTransmission,0.8,0.14,90);

  c1->cd(2);
  char hNameY[200]; strcpy(hNameY,"h"); strcat(hNameY,name); strcat(hNameY,"y");
  cout<<hNameY<<endl;
  TH1D* hisY = new TH1D(hNameY,hNameY,100,-25,25);
  hisY->GetXaxis()->SetTitle("y (cm)");
  char whatToPlotY[200]; strcpy(whatToPlotY,"save_y/10>>"); strcat(whatToPlotY,hNameY);
  t1->Draw(whatToPlotY,condition);

  c1->cd(3);
  char hNameXX[200]; strcpy(hNameXX,"h"); strcat(hNameXX,name); strcat(hNameXX,"Xprime");
  cout<<hNameXX<<endl;
  TH1D* hisXX = new TH1D(hNameXX,hNameXX,100,-100,100);
  hisXX->GetXaxis()->SetTitle("x_{angle} (mrad)");
  char whatToPlotXX[200]; strcpy(whatToPlotXX,"atan2(save_px,save_pz)*1000>>"); strcat(whatToPlotXX,hNameXX);
  cout<<whatToPlotXX<<endl;
  t1->Draw(whatToPlotXX,condition);

  c1->cd(4);
  char hNameYY[200]; strcpy(hNameYY,"h"); strcat(hNameYY,name); strcat(hNameYY,"Yprime");
  cout<<hNameYY<<endl;
  TH1D* hisYY = new TH1D(hNameYY,hNameYY,100,-100,100);
  hisYY->GetXaxis()->SetTitle("y_{angle} (mrad)");
  char whatToPlotYY[200]; strcpy(whatToPlotYY,"atan2(save_py,save_pz)*1000>>"); strcat(whatToPlotYY,hNameYY);
  cout<<whatToPlotYY<<endl;
  t1->Draw(whatToPlotYY,condition);

  //  c1->cd(5);
  //  char hNamePolT[200]; strcpy(hNamePolT,"h"); strcat(hNamePolT,name); strcat(hNamePolT,"PolT");
  //  cout<<hNamePolT<<endl;
  //  TH1D* hisPolT = new TH1D(hNamePolT,hNamePolT,102,-1.02,1.02);
  //  hisPolT->GetXaxis()->SetTitle("#sqrt{(P_{x}^{2}+P_{y}^{2})}");
  //  char whatToPlotPolT[200]; strcpy(whatToPlotPolT,"sqrt(save_polx*save_polx+save_poly*save_poly)>>"); strcat(whatToPlotPolT,hNamePolT);
  //  cout<<whatToPlotPolT<<endl;
  //  t1->Draw(whatToPlotPolT,condition);

  c1->cd(5);
  char hNameThetaPol[200]; strcpy(hNameThetaPol,"h"); strcat(hNameThetaPol,name); strcat(hNameThetaPol,"ThetaPol");
  cout<<hNameThetaPol<<endl;
  TH1D* hisThetaPol = new TH1D(hNameThetaPol,hNameThetaPol,90,-90.,90);
  hisThetaPol->GetXaxis()->SetTitle("#theta(P)-90\,deg");
  //  char whatToPlotPolT[200]; strcpy(whatToPlotPolT,"180/3.14159*atan(sqrt(save_polx*save_polx+save_poly*save_poly)/save_polz)>>"); strcat(whatToPlotPolT,hNameThetaPol);
  char whatToPlotPolT[200]; strcpy(whatToPlotPolT,"180/3.14159*atan(save_polz/sqrt(save_polx*save_polx+save_poly*save_poly))>>"); strcat(whatToPlotPolT,hNameThetaPol);
  cout<<whatToPlotPolT<<endl;
  t1->Draw(whatToPlotPolT,condition);
  
  c1->cd(6);
  char hNamePz[200]; strcpy(hNamePz,"h"); strcat(hNamePz,name); strcat(hNamePz,"pz");
  TH1D* hisPz = new TH1D(hNamePz,hNamePz,100,-50,50);
  hisPz->GetXaxis()->SetTitle("(p-p0)/p0 (%)");
  char whatToPlotPz[200]; strcpy(whatToPlotPz,"(save_pz-27.4)/27.4*100>>"); strcat(whatToPlotPz,hNamePz);
  t1->Draw(whatToPlotPz,condition);

  return c1;
}

TCanvas* PlotOneSaveVolumeTwo(char* name, char* condition) {
  cout<<"PlotOneSaveVolume called"<<endl;
  char cName[100]; strcpy(cName,"c"); strcat(cName,name);
  TCanvas* c1= new TCanvas(cName,cName,700,900);
  c1->Divide(2,3);

  c1->cd(1);
  char hName1[200]; strcpy(hName1,"h"); strcat(hName1,name); strcat(hName1,"xXxprime");
  TH2D* his1 = new TH2D(hName1,hName1,40,-25,25,40,-25,25);
  his1->GetXaxis()->SetTitle("x (cm)");
  his1->GetYaxis()->SetTitle("y (cm)");
  char whatToPlotX[200]; strcpy(whatToPlotX,"save_y/10:save_x/10>>"); strcat(whatToPlotX,hName1);
  t1->Draw(whatToPlotX,condition,"box");
  if (strcmp(popis,"undefined")) {
    Vysvetlivka(popis,0.2,0.14,90);
  }

  c1->cd(2);
  char hName2[200]; strcpy(hName2,"h"); strcat(hName2,name); strcat(hName2,"xXxprime");
  TH2D* his2 = new TH2D(hName2,hName2,40,-25,25,40,-100,100);
  his2->GetXaxis()->SetTitle("x (cm)");
  his2->GetYaxis()->SetTitle("x_{angle} (mrad)");
  char whatToPlotX[200]; strcpy(whatToPlotX,"(atan2(save_px,save_pz)*1000):save_x/10>>"); strcat(whatToPlotX,hName2);
  t1->Draw(whatToPlotX,condition,"box");

  c1->cd(3);
  char hName3[200]; strcpy(hName3,"h"); strcat(hName3,name); strcat(hName3,"xXp");
  TH2D* his3 = new TH2D(hName3,hName3,40,-25,25,40,-50,50);
  his3->GetXaxis()->SetTitle("x (cm)");
  his3->GetYaxis()->SetTitle("(p-p0)/p0 (%)");
  char whatToPlotX[200]; strcpy(whatToPlotX,"((save_pz-27.4)/27.4*100):(save_x/10)>>"); strcat(whatToPlotX,hName3);
  t1->Draw(whatToPlotX,condition,"box");

  return c1;
}

void FillEnvelope(TH1D* hist, const Double_t z, char* velicina, char* condition) {
  TH1D hh = TH1D("hh","hh",1000,-50.,50.);
  t1->Draw(velicina,condition);
  Double_t RMS2 = 2*(hh.GetRMS());
  if (strcmp(velicina,"save_y/10>>hh")==0) {RMS2 *=-1;}
  std::cout<<"z="<<z<<"  RMS2="<<RMS2<<std::endl;
  hist->Fill(z,RMS2);
}

void Vysvetlivka(char* textik, Double_t x, Double_t y, Double_t uhel=270, Double_t textSize=0.04) {
  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(uhel);
  latex.SetTextSize(textSize);
  latex.DrawLatex(x,y,textik);
}
