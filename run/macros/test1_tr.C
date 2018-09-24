// To run root without logon screen and script use: root -l -n
// To run a named macro in root use:
//.L test1_tr.C
// sr_transm("data/musr_1100.root", 868)

#include <string>
void sr_transm(const char* fname, int saveID)
{
  char cutStr[128];
  TH1D  *pol_x, *pol_z, *xisect, *yisect, *xfsect, *yfsect;
  TH2D  *ini_hist, *bspot_hist, *bspot_hist1;
  TTree *t1;
    
  TFile* f1 = new TFile(fname);
  t1 = (TTree*) gDirectory->FindObjectAny("t1");

  sprintf(cutStr,"save_detID== %3i &&save_particleID==-13 && save_pz>0",saveID);
  gStyle->SetPalette(1,0);
  TCanvas* c1 = new TCanvas("c1","Polarization and Transmission",60,40,600,800);
  c1->Divide(2,3);
  c1->SetGridx();
  c1->SetGridy();

  // Resulting polarization in the x direction
  c1->cd(1);
  t1->Draw("save_polx >> pol_x(128,-1,1)",cutStr); //0, 2
  pol_x = (TH1D*) gDirectory->Get("pol_x");
  pol_x->SetTitle("Polarization X");
  pol_x->GetXaxis()->SetNdivisions(405);
  pol_x->GetYaxis()->SetNdivisions(406);
  pol_x->GetXaxis()->SetTickLength(0.018);
  pol_x->GetYaxis()->SetTickLength(0.018);
  pol_x->Draw("F");
  pol_x->SetFillStyle(1001);
  pol_x->SetFillColor(kGreen-5);
  pol_x->Draw();
  //c1->SaveAs("data/sr_bspot_0.eps");

  // Resulting polarization in the z direction
  c1->cd(2);
  t1->Draw("save_polz >> pol_z(128, -1, 1)",cutStr); //-1., 1
  pol_z = (TH1D*) gDirectory->Get("pol_z");
  pol_z->SetTitle("Polarization Z");
  pol_z->GetXaxis()->SetNdivisions(405);
  pol_z->GetYaxis()->SetNdivisions(406);
  pol_z->GetXaxis()->SetTickLength(0.018);
  pol_z->GetYaxis()->SetTickLength(0.018);
  pol_z->Draw("F");
  pol_z->SetFillStyle(1001);
  pol_z->SetFillColor(38);
  pol_z->Draw();


  // Initial beam spot
  c1->cd(3)->SetGridx();c1->cd(3)->SetGridy(); // c2->cd(3);
//   t1->Draw("muIniPosY[0]:muIniPosX[0]>>ini_hist(64,-40, 40,64,-40,40)");
  t1->Draw("muIniPosY[0]:muIniPosZ[0]>>ini_hist(64,-1710,-1646,64,-40,40)","muIniPolZ > -2");
  ini_hist = (TH2D*) gDirectory->Get("ini_hist");
  ini_hist->SetTitle("Muon initial beam cross section [mm]");
  ini_hist->Draw();
  ini_hist->Draw("cont0 same");
  ini_hist->ProjectionX("xisect",31,33);
  ini_hist->ProjectionY("yisect",31,33);
  
  // Resulting beam spot
  c1->cd(4)->SetGridx();c1->cd(4)->SetGridy(); // c1->cd(3);
  // t1->Draw("save_y:save_x>>bspot_hist(64,-40,40,64,-40,40)","save_detID==902&&save_particleID==-13");
  t1->Draw("save_y:save_x>>bspot_hist(80,-40,40,80,-40,40)",cutStr);
  bspot_hist = (TH2D*) gDirectory->Get("bspot_hist");
  bspot_hist->SetTitle("Muon beam cross section [mm]");
/*  bspot_hist->GetXaxis()->SetTitle("x at z=16 [mm]")
  bspot_hist->GetYaxis()->SetTitle("y at z=16 [mm]")*/
  bspot_hist->Draw();
  bspot_hist->Draw("cont0 same");
  bspot_hist->ProjectionX("xfsect",31,33);
  bspot_hist->ProjectionY("yfsect",31,33);

  // Initial beam spot cross section
  c1->cd(5);
  xisect = (TH1D*) gDirectory->Get("xisect");
  xisect->SetTitle("x initial cross section [mm]");
  xisect->SetFillStyle(1001);
  xisect->SetFillColor(45);
  xisect->Draw();
   
  // Resulting beam spot cross section
  c1->cd(6);
  c1->cd(6)->SetGridx();c1->cd(6)->SetGridy();
  t1->Draw("save_y:save_x>>bspot_hist1(80,-20,20,80,-20,20)",cutStr);
  bspot_hist1 = (TH2D*) gDirectory->Get("bspot_hist1");
  bspot_hist1->SetTitle("Muon beam cross section [mm]");
  bspot_hist1->GetXaxis()->SetTitle("x (mm)");
  bspot_hist1->GetYaxis()->SetTitle("y (mm)");
  bspot_hist1->Draw();
  bspot_hist1->Draw("cont0 same");
  cout << "Mean x, Mean y, RMS x, RMS y: " << bspot_hist1->GetMean(1) << ", " << bspot_hist1->GetMean(2) << ", " <<  bspot_hist1->GetRMS(1) << ", " << bspot_hist1->GetRMS(2) << ", " << endl;
  cout << "Fraction in [-10.,10.]: " << bspot_hist->Integral(31,50,31,50)/bspot_hist->Integral(1,80,1,80) << endl;
  cout << "Fraction in [-5.0,5.0]: " << bspot_hist->Integral(36,45,36,45)/bspot_hist->Integral(1,80,1,80) << endl;

  cout << "Fraction in [-2.5,2.5]: " << bspot_hist1->Integral(36,45,36,45)/bspot_hist->Integral(1,80,1,80) << endl;
}

void phase_space(const char* fname)
{
  TH1D  *pol_x, *pol_z, *xisect, *yisect, *xfsect, *yfsect;
  TH2D  *ini_hist, *bspot_hist, *bspot_hist1;
  TTree *t1;

  TFile* f1 = new TFile(fname);
  t1 = (TTree*) gDirectory->FindObjectAny("t1");

  gStyle->SetPalette(1,0);
  TCanvas* c1 = new TCanvas("c1","Phase Space",60,40,600,800);
  c1->Divide(2,2);
  c1->SetGridx();
  c1->SetGridy();

  // Resulting phase space px,py
  c1->cd(1);
  t1->Draw("muIniMomY[0]:muIniMomX[0]>>ini_hist(64,-40,40,64,-40,40)","muIniPolX > -2");
  ini_hist->SetTitle("Muon initial beam phase space []");
  ini_hist->Draw();
  ini_hist->Draw("cont0 same");
  ini_hist->ProjectionX("xisect",31,33);
  ini_hist->ProjectionY("yisect",31,33);

  // Resulting polarization in the z direction
  c1->cd(2);
  t1->Draw("muTargetPosY[0]:muTargetPosX[0]>>bspot_hist(64,-40,40,64,-40,40)","muTargetPolX > -2");
  bspot_hist->SetTitle("Muon beam cross section [mm]");
  bspot_hist->Draw();
  bspot_hist->Draw("cont0 same");
  bspot_hist->ProjectionX("xfsect",31,33);
  bspot_hist->ProjectionY("yfsect",31,33);

  // Initial beam spot
  c1->cd(3)->SetGridx();c1->cd(3)->SetGridy(); // c2->cd(3);
  t1->Draw("muIniPosY[0]:muIniPosX[0]>>ini_hist(64,-40,40,64,-40,40)","muIniPolX > -2");
  ini_hist->SetTitle("Muon initial beam cross section [mm]");
  ini_hist->Draw();
  ini_hist->Draw("cont0 same");
  ini_hist->ProjectionX("xisect",31,33);
  ini_hist->ProjectionY("yisect",31,33);
  
  // Resulting beam spot
  c1->cd(4)->SetGridx();c1->cd(4)->SetGridy(); // c1->cd(3);
  t1->Draw("muTargetPosY[0]:muTargetPosX[0]>>bspot_hist(64,-40,40,64,-40,40)","muTargetPolX > -2");
  bspot_hist->SetTitle("Muon beam cross section [mm]");
  bspot_hist->Draw();
  bspot_hist->Draw("cont0 same");
  bspot_hist->ProjectionX("xfsect",31,33);
  bspot_hist->ProjectionY("yfsect",31,33);

}

void Slice(const char* fname,  Int_t Slice)
{
  TH1D  *pol_x, *pol_z, *xisect, *yisect, *xfsect, *yfsect, *zhist;
  TH2D  *ini_hist, *bspot_hist;
  TTree *t1;

  TFile* f1 = new TFile(fname);
  TString Cond,Title;
  Double_t zSlice;
  
  t1 = (TTree*) gDirectory->FindObjectAny("t1");

  gStyle->SetPalette(1,0);
  TCanvas* c1 = new TCanvas("c1","Polarization and Transmission",60,40,600,800);
  c1->Divide(2,3);
  c1->SetGridx();
  c1->SetGridy();

  // Resulting polarization in the x direction
  c1->cd(1);
  Cond.Form("save_detID== %04d&&save_particleID==-13",Slice);
  t1->Draw("save_polx >> pol_x(64,-1,1)",Cond); //0, 2
  pol_x->SetTitle("Polarization X");
  pol_x->GetXaxis()->SetNdivisions(405);
  pol_x->GetYaxis()->SetNdivisions(406);
  pol_x->GetXaxis()->SetTickLength(0.018);
  pol_x->GetYaxis()->SetTickLength(0.018);
  pol_x->Draw("F");
  pol_x->SetFillStyle(1001);
  pol_x->SetFillColor(kGreen-5);
  pol_x->Draw();
  //c1->SaveAs("data/sr_bspot_0.eps");

  // Resulting polarization in the z direction
  c1->cd(2);
  t1->Draw("save_polz >> pol_z(64,-1,1)",Cond); //-1., 1
  pol_z->SetTitle("Polarization Z");
  pol_z->GetXaxis()->SetNdivisions(405);
  pol_z->GetYaxis()->SetNdivisions(406);
  pol_z->GetXaxis()->SetTickLength(0.018);
  pol_z->GetYaxis()->SetTickLength(0.018);
  pol_z->Draw("F");
  pol_z->SetFillStyle(1001);
  pol_z->SetFillColor(38);
  pol_z->Draw();


  // Initial beam spot
  c1->cd(3)->SetGridx();c1->cd(3)->SetGridy(); // c2->cd(3);
  t1->Draw("muIniPosZ[0]:muIniPosY[0]>>ini_hist(64,-40,40,64,-1710,-1646)","muIniPolZ > -2");
  ini_hist->SetTitle("Muon initial beam cross section [mm]");
  ini_hist->Draw();
  ini_hist->Draw("cont0 same");
  ini_hist->ProjectionX("xisect",31,33);
  ini_hist->ProjectionY("yisect",31,33);
  
  // Resulting beam spot
  c1->cd(4)->SetGridx();c1->cd(4)->SetGridy(); // c1->cd(3);
  t1->Draw("save_z>>zhist",Cond);
  zSlice=zhist->GetMean(1);
  t1->Draw("save_y:save_x>>bspot_hist(64,-40,40,64,-40,40)",Cond);
  Title.Form("Muon beam cross section [mm], z=%g [mm]",zSlice);
  bspot_hist->SetTitle(Title);
  bspot_hist->Draw();
  bspot_hist->Draw("cont0 same");
  bspot_hist->ProjectionX("xfsect",31,33);
  bspot_hist->ProjectionY("yfsect",31,33);

  // Initial beam spot cross section
  c1->cd(5);
  xisect->SetTitle("x initial cross section [mm]");
  xisect->SetFillStyle(1001);
  xisect->SetFillColor(45);
  xisect->Draw();

    
  // Resulting beam spot cross section
  c1->cd(6);
  xfsect->SetTitle("x cross section [mm]");
  xfsect->SetFillStyle(1001);
  xfsect->SetFillColor(45);
  xfsect->Draw();
}


void VarSlice(const char* fname,  char* Var, Int_t Slice, Int_t SubPlot)
{
  TH1D  *histo;
  TTree *t1;

  TFile* f1 = new TFile(fname);
  t1 = (TTree*) gDirectory->FindObjectAny("t1");

  TString Cond,Title,Histo;
  Double_t zSlice;
  

  gStyle->SetPalette(1,0);
  TCanvas* c1 = new TCanvas("c1", Var);
  //  c1->Divide(3,5);
  c1->cd(SubPlot);

  Cond.Form("save_detID== %04d&&save_particleID==-13",Slice);
  Histo.Form("%s >> histo",Var);
  t1->Draw(Histo,Cond); 
  histo->SetTitle(Var);
  histo->Draw("F");
  histo->SetFillStyle(1001);
  histo->SetFillColor(kGreen-5);
  histo->Draw();
}

void VarxySlice(const char* fname,  char* Varx, char* Vary, Int_t Slice, Int_t SubPlot)
{
  TH1D  *histo;
  TTree *t1;

  TFile* f1 = new TFile(fname);
  TString Cond,Title,Histo;
  Double_t zSlice;
  
  t1 = (TTree*) gDirectory->FindObjectAny("t1");

  gStyle->SetPalette(1,0);
  TCanvas* c1 = new TCanvas("c1", Varx);
  //  c1->Divide(3,5);
  c1->cd(SubPlot);

  Cond.Form("save_detID== %04d&&save_particleID==-13",Slice);
  Histo.Form("%s:%s >> histo",Vary,Varx);
  t1->Draw(Histo,Cond); 
  histo->SetTitle(Histo);
  histo->Draw();
  histo->Draw("cont0 same");
}

void Beam_Env(const char* fname,  Int_t iSlice, Int_t fSlice)
{
  TH2D *bspot_hist;
  TTree *t1;

  TFile* f1 = new TFile(fname);
  TString Cond,Title;
  Double_t zSlice,Xrms,Yrms,Xavg,Yavg,Nbins;
  Int_t i;

  t1 = (TTree*) gDirectory->FindObjectAny("t1");
  
  gStyle->SetPalette(1,0);
  TCanvas* c1 = new TCanvas("c1","Polarization and Transmission");

  // Initial beam spot
//   t1->Draw("muIniPosZ[0]:muIniPosY[0]:muIniPosX[0]>>ini_hist(64,-40,40,64,-40,40)","muIniPolX > -2");
//   ini_hist->SetTitle("Muon initial beam cross section [mm]");
//   ini_hist->Draw();
//   ini_hist->Draw("cont0 same");
//   ini_hist->ProjectionX("xisect",31,33);
//   ini_hist->ProjectionY("yisect",31,33);
  
  printf("SliceID \t Z \t Xrms \t Yrms \t Xavg \t Yavg \t N\n");
  for (i=iSlice; i<=fSlice; i++) 
    {
      // Resulting beam spot
      Cond.Form("save_detID==%d&&save_particleID==-13",i);
      //      Cond.Form("save_detID==%d&& (save_particleID!=22) ",i);
      t1->Draw("save_z:save_y:save_x>>bspot_hist(64,-80,80,64,-80,80,100,-2600,300)",Cond);
      //      t1->Draw("save_z:save_y:save_x>>bspot_hist(64,-40,40,64,-40,40,100,-290,290)",Cond);
      bspot_hist = (TH2D*) gDirectory->Get("bspot_hist");
      // Get properties of slice
      zSlice=bspot_hist->GetMean(3);

      Xrms=bspot_hist->GetRMS(1);
      Yrms=bspot_hist->GetRMS(2);
      
      Xavg=bspot_hist->GetMean(1);
      Yavg=bspot_hist->GetMean(2);
      
      Nbins=bspot_hist->Integral();
      
      printf("%d\t%g\t%g\t%g\t%g\t%g\t%g\n",i,zSlice,Xrms,Yrms,Xavg,Yavg,Nbins);
      //      delete bspot_hist;
    }
  //  t1->Draw("save_z:save_y:save_x>>bspot_hist(64,-40,40,64,-40,40,100,-290,290)","save_particleID==-13");
  //  t1->Draw("save_z:save_y:save_x>>bspot_hist(64,-40,40,64,-40,40,100,-2600,300)","save_particleID==-13");
  //  bspot_hist->Draw("Iso");
}

