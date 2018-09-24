// To run root without logon screen and script use: root -l -n
// To run a named macro in root use:
//.L sr_tr.C
// sr_transm("data/sr1_1100.root")

#include <string>
void sr_transm(char* fname)
{

  TFile* f1 = new TFile(fname);

  gStyle->SetPalette(1,0);
  TCanvas* c1 = new TCanvas("c1","Polarization and Transmission",60,40,600,800);
  c1->Divide(2,3);
  c1->SetGridx();
  c1->SetGridy();

  // Resulting polarization in the x direction
  c1->cd(1);
  t1->Draw("save_polx >> pol_x(128, -1, 1)","save_detID==902&&save_particleID==-13"); //0, 2
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
  t1->Draw("save_polz >> pol_z(128, -1, 1)","save_detID==902&&save_particleID==-13"); //-1., 1
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
  // t1->Draw("save_y:save_x>>bspot_hist(64,-40,40,64,-40,40)","save_detID==902&&save_particleID==-13");
  t1->Draw("save_y:save_x>>bspot_hist(64,-40,40,64,-40,40)","save_detID==902&&save_particleID==-13&&sqrt(save_x**2+save_y**2)<35");
  bspot_hist->SetTitle("Muon beam cross section [mm]");
/*  bspot_hist->GetXaxis()->SetTitle("x at z=16 [mm]")
  bspot_hist->GetYaxis()->SetTitle("y at z=16 [mm]")*/
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
/*  c1->cd(6)->SetGridx();c1->cd(6)->SetGridy();
  t1->Draw("save_y:save_x>>bspot_hist1(64,-40,40,64,-40,40)","save_detID==901&&save_particleID==-13&&sqrt(save_x**2+save_y**2)<=15");
  bspot_hist1->SetTitle("Muon beam cross section [mm]");
  bspot_hist1->GetXaxis()->SetTitle("x at z=0 (mm)")
  bspot_hist1->GetYaxis()->SetTitle("y at z=0 (mm)")
  bspot_hist1->Draw();
  bspot_hist1->Draw("cont0 same");*/
  xfsect->SetTitle("x cross section [mm]");
  xfsect->SetFillStyle(1001);
  xfsect->SetFillColor(45);
  xfsect->Draw();
}

void phase_space(char* fname)
{

  TFile* f1 = new TFile(fname);

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

void Slice(char* fname,  Int_t Slice)
{

  TFile* f1 = new TFile(fname);
  TString Cond,Title;
  Double_t zSlice;
  

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


void VarSlice(char* fname,  char* Var, Int_t Slice, Int_t SubPlot)
{

  TFile* f1 = new TFile(fname);
  TString Cond,Title,Histo;
  Double_t zSlice;
  

  gStyle->SetPalette(1,0);
  //  TCanvas* c1 = new TCanvas("c1",Var);
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

void VarxySlice(char* fname,  char* Varx, char* Vary, Int_t Slice, Int_t SubPlot)
{

  TFile* f1 = new TFile(fname);
  TString Cond,Title,Histo;
  Double_t zSlice;
  

  gStyle->SetPalette(1,0);
  //  TCanvas* c1 = new TCanvas("c1",Var);
  //  c1->Divide(3,5);
  c1->cd(SubPlot);

  Cond.Form("save_detID== %04d&&save_particleID==-13",Slice);
  Histo.Form("%s:%s >> histo",Vary,Varx);
  t1->Draw(Histo,Cond); 
  histo->SetTitle(Histo);
  histo->Draw();
  histo->Draw("cont0 same");
}

void Beam_Env(char* fname,  Int_t iSlice, Int_t fSlice)
{

  TFile* f1 = new TFile(fname);
  TString Cond,Title;
  Double_t zSlice,Xrms,Yrms,Xavg,Yavg,Nbins;
  Int_t i;
  
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
      t1->Draw("save_z:save_y:save_x>>bspot_hist(64,-40,40,64,-40,40,100,-2600,300)",Cond);
      //      t1->Draw("save_z:save_y:save_x>>bspot_hist(64,-40,40,64,-40,40,100,-290,290)",Cond);

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

