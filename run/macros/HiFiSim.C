#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <string>

void EnvatZ(Int_t fnumber, char* fname, TCanvas* c1)
{
  // For old lem4 simulation files
  TFile* f1 = new TFile(fname);

  //DEFINE HISTOGRAMS
  TH3F* hBeamSpot   = new TH3F("hBeamSpot", " x,y,z", 32, -100., 100., 32, -100., 100.,2900,-900,2000);
  //  TH1F* hPosZ       = new TH1F("hPosZ", "z",1600,-800.,800.);

//   hBeamSpot->Fill(muTargetPosX[0], muTargetPosY[0]);
//   hPosZ->Fill(muTargetPosZ[0]);
    
  //  TCanvas* c1= new TCanvas("c1","canvas 1");
  //  hPosZ->Draw();

//   gStyle->SetPalette(1,0);
//  TCanvas* c1= new TCanvas("c1","canvas 1");
//  c1->cd(1);
  t1->Draw("muTargetPosZ[0]:muTargetPosY[0]:muTargetPosX[0]>>hBeamSpot");
  //  hBeamSpot->Draw("lego");
//   hBeamSpot->Draw();
  hBeamSpot->DrawCopy("cont0 same");

  Double_t Xrms=hBeamSpot->GetRMS(1);
  Double_t Yrms=hBeamSpot->GetRMS(2);
  Double_t Zrms=hBeamSpot->GetRMS(3);

  Double_t Xavg=hBeamSpot->GetMean(1);
  Double_t Yavg=hBeamSpot->GetMean(2);
  Double_t Zavg=hBeamSpot->GetMean(3);

  Double_t Nbins=hBeamSpot->Integral();
  //  Long64_t Nbins=t1.save_x->GetEntries();
  if (Xrms != 0) {
    printf("%d\t%g\t%g\t%g\t%g\t%g\t%g\n",fnumber,Zavg,Xrms,Yrms,Xavg,Yavg,Nbins);
  }

  // Clean up
  //  delete c1;
  delete f1;


  //  hPosZ->Draw();
  
  //   sprintf(filenamePrint,"musrSrClass_%s.ps",fileNr);
  //   c1->Print(filenamePrint);
}

void BeamSlice(char* fname,Int_t SaveID)
{

  Double_t zSlice,Xrms,Yrms,Xavg,Yavg,Nbins;
  // For new musrSim simulation files
  TFile* f1 = new TFile(fname);
  TString Cond;
  // Define an array with save volume IDs
  Cond.Form("save_detID== %04d&&save_particleID==-13",SaveID);
  t1->Draw("save_z:save_y:save_x>>bspot_hist(32,-100,100,32,-100,100,100,-2600,1300)",Cond);
  //  t1->Draw("save_y:save_x>>bspot_hist(64,-40,40,64,-40,40)",Cond);
  gStyle->SetPalette(1,0);
  bspot_hist->SetTitle("Beam Cross Section [mm]");
  bspot_hist->Draw();
  bspot_hist->Draw("cont0 same");

  zSlice=bspot_hist->GetMean(3);
  Xrms=bspot_hist->GetRMS(1);
  Yrms=bspot_hist->GetRMS(2);
  Xavg=bspot_hist->GetMean(1);
  Yavg=bspot_hist->GetMean(2);
  Nbins=bspot_hist->Integral();
      
  printf("%d\t%g\t%g\t%g\t%g\t%g\t%g\n",SaveID,zSlice,Xrms,Yrms,Xavg,Yavg,Nbins);

  f1->Close();
}

void Slice(char* fname,Int_t SaveID)
{
  // For new musrSim simulation files
  TFile* f1 = new TFile(fname);
  TString Cond;
  // Define an array with save volume IDs
  Cond.Form("save_detID== %04d&&save_particleID==-13",SaveID);

  gStyle->SetPalette(1,0);
  TCanvas* c1 = new TCanvas("c1","Slice Properties",60,40,600,800);
  c1->Divide(2,3);

  // Initial beam spot
  c1->cd(1);
  t1->Draw("muIniPosY[0]:muIniPosX[0]>>ini_hist(32,-100,100,32,-100,100)");
  ini_hist->SetTitle("Muon initial beam cross section [mm]");
  ini_hist->Draw();
  ini_hist->Draw("cont0 same");
  ini_hist->ProjectionX("xisect",31,33);
  ini_hist->ProjectionY("yisect",31,33);

  // Beam spot at slice
  c1->cd(2);
  t1->Draw("save_y:save_x>>bspot_hist(32,-100,100,32,-100,100)",Cond);
      //  t1->Draw("save_y:save_x>>bspot_hist(64,-40,40,64,-40,40)",Cond);
  bspot_hist->SetTitle("Beam Cross Section [mm]");
  bspot_hist->Draw();
  bspot_hist->Draw("cont0 same");

  // Initial x polarization
  c1->cd(3);
  t1->Draw("muIniPolX >> ipol_x(128, -1.1, 1.1)"); //0, 2
  ipol_x->SetTitle("Polarization X");
  ipol_x->GetXaxis()->SetNdivisions(405);
  ipol_x->GetYaxis()->SetNdivisions(406);
  ipol_x->GetXaxis()->SetTickLength(0.018);
  ipol_x->GetYaxis()->SetTickLength(0.018);
  ipol_x->Draw("F");
  ipol_x->SetFillStyle(1001);
  ipol_x->SetFillColor(kGreen-5);
  ipol_x->Draw();
  

  // x polarization at slice
  c1->cd(4);
  t1->Draw("save_polx >> pol_x(128, -1.1, 1.1)",Cond); //0, 2
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

  // Initial z polarization
  c1->cd(5);
  t1->Draw("muIniPolZ >> ipol_z(128, -1.1, 1.1)"); //0, 2
  ipol_z->SetTitle("Polarization Z");
  ipol_z->GetXaxis()->SetNdivisions(405);
  ipol_z->GetYaxis()->SetNdivisions(406);
  ipol_z->GetXaxis()->SetTickLength(0.018);
  ipol_z->GetYaxis()->SetTickLength(0.018);
  ipol_z->Draw("F");
  ipol_z->SetFillStyle(1001);
  ipol_z->SetFillColor(kGreen-5);
  ipol_z->Draw();

  // z polarization at slice
  c1->cd(6);
  t1->Draw("save_polz >> pol_z(128, -1.1, 1.1)",Cond); //-1., 1
  pol_z->SetTitle("Polarization Z");
  pol_z->GetXaxis()->SetNdivisions(405);
  pol_z->GetYaxis()->SetNdivisions(406);
  pol_z->GetXaxis()->SetTickLength(0.018);
  pol_z->GetYaxis()->SetTickLength(0.018);
  pol_z->Draw("F");
  pol_z->SetFillStyle(1001);
  pol_z->SetFillColor(38);
  pol_z->Draw();

  // f1->Close();
}

void CompSlice(Int_t ifnamei, Int_t ifnamef, Int_t SaveID)
{
  TString fname;
  TString title;
  TString Cond;
  Double_t B,Xrms,Yrms,Xavg,Yavg,Nbins;

  Cond.Form("save_detID== %04d&&save_particleID==-13&&save_n==1",SaveID);

  gStyle->SetPalette(1,0);
  TCanvas* c1 = new TCanvas("c1","Slice Properties",60,40,600,800);
  c1->Divide(4,4);

  Int_t ifname;
  Int_t counter=0;

  printf("B \t Xrms \t Yrms \t Xavg \t Yavg \t N\n");
  for (ifname=ifnamei; ifname<=ifnamef; ifname++) {
    counter++;
    fname.Form("data/musr_%d.root",ifname);
    TFile* f1 = new TFile(fname);
    // Initial beam spot
    c1->cd(counter);
    t1->Draw("save_y:save_x>>bspot_hist(32,-100,100,32,-100,100)",Cond);
    B=(ifname-ifnamei)*0.05+0.6;
    title.Form("B=%f Tesla",B);
    bspot_hist->SetTitle(title);
    bspot_hist->Draw();
    bspot_hist->Draw("cont0 same");
    Xrms=bspot_hist->GetRMS(1);
    Yrms=bspot_hist->GetRMS(2);
    Xavg=bspot_hist->GetMean(1);
    Yavg=bspot_hist->GetMean(2);
    Nbins=bspot_hist->Integral();

    printf("%g\t%g\t%g\t%g\t%g\t%g\n",B,Xrms,Yrms,Xavg,Yavg,Nbins);

  // f1->Close();
  }
}


void BeamSlices(char* fname,Int_t SaveIDi, Int_t SaveIDf)
{
  // For new musrSim simulation files
  TString Cond;
  Int_t i;

  printf("SliceID \t Z \t Xrms \t Yrms \t Xavg \t Yavg \t N\n");
  for (i=SaveIDi; i<=SaveIDf; i++) {

    BeamSlice(fname,i);
  }

}

void NatSlice(char* fname,Int_t SaveID)
{
  // For new musrSim simulation files
  TString Cond;
  Double_t zSlice,Xrms,Yrms,Xavg,Yavg,Nbins;
  Int_t i;
  TFile* f1 = new TFile(fname);
  TString Cond;

  // Define an array with save volume IDs
  Cond.Form("save_detID== %04d&&save_particleID==-13",SaveID);
  t1->Draw("save_z:save_y:save_x>>bspot_hist(32,-100,100,32,-100,100,100,-2600,1300)",Cond);
  //  t1->Draw("save_y:save_x>>bspot_hist(64,-40,40,64,-40,40)",Cond);
  gStyle->SetPalette(1,0);
  bspot_hist->SetTitle("Beam Cross Section [mm]");
  bspot_hist->Draw();
  bspot_hist->Draw("cont0 same");

  zSlice=bspot_hist->GetMean(3);
  Xrms=bspot_hist->GetRMS(1);
  Yrms=bspot_hist->GetRMS(2);
  Xavg=bspot_hist->GetMean(1);
  Yavg=bspot_hist->GetMean(2);
  Nbins=bspot_hist->Integral();
      
  //    printf("%d\t%g\t%g\t%g\t%g\t%g\t%g\n",i,zSlice,Xrms,Yrms,Xavg,Yavg,Nbins);
  printf("%d\t",Nbins);
  f1->Close();
  
}
