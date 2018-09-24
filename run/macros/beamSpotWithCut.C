// To run root without logon screen and script use: root -l -n
// To run a named macro in root use:
//.L beamSpotWithCut.C
// beamSpotWithCut("data/sr1_1100.root", 10)

#include <string>
#define NSIZE 100000
void beamSpotWithCut(char* fname, Double_t cut)
{
  Double_t *pTDx, *pTDy, *pSamplex, *pSampley, *pEventIDTD, *pEventIDSample;
  Double_t TDx[NSIZE], TDy[NSIZE], Samplex[NSIZE], Sampley[NSIZE], EventIDTD[NSIZE], EventIDSample[NSIZE];
  Long64_t i, j, k, nEventsTD, nEventsSample;
  TFile* f1 = new TFile(fname);

  // muon beam spot at TD
  t1->Draw("eventID:save_y:save_x","save_detID==890&&save_particleID==-13"); //0, 2
  nEventsTD  = t1->GetSelectedRows();
  pTDx       = t1->GetV3();
  pTDy       = t1->GetV2();
  pEventIDTD = t1->GetV1();

  for (i = 0; i<nEventsTD; i++){
   EventIDTD[i] = pEventIDTD[i];
   TDx[i]     = pTDx[i];
   TDy[i]     = pTDy[i];   
  }

  // muon beam spot at Sample
  t1->Draw("eventID:save_y:save_x","save_detID==902&&save_particleID==-13"); //0, 2
  nEventsSample  = t1->GetSelectedRows();
  pSamplex       = t1->GetV3();
  pSampley       = t1->GetV2();
  pEventIDSample = t1->GetV1();

  for (i = 0; i<nEventsSample; i++){
   EventIDSample[i] = pEventIDSample[i];
   Samplex[i]       = pSamplex[i];
   Sampley[i]       = pSampley[i];   
  }

  // muon beam spot at Sample with cut
  TH2D *hBeamSpot = new TH2D("beam spot with cut", "beam spot with cut", 60, -30., 30., 60, -30., 30);
  k = 0;
 for (i = 0; i<nEventsSample; i++){
   for (j = k; j<nEventsTD; j++){
//    cout << i << ", " << j << ", " << k << ", " << EventIDSample[i] << ", " << EventIDTD[j] << ", " << Samplex[i] << ", " << sqrt(TDx[j]*TDx[j]+TDy[j]*TDy[j]) << endl;
     if (EventIDSample[i] == EventIDTD[j]){
       if (sqrt(TDx[j]*TDx[j]+TDy[j]*TDy[j])<=cut) hBeamSpot->Fill(Samplex[i], Sampley[i]);
        k = j + 1;
        break;
     }
   }
  }

  gStyle->SetPalette(1,0);
//  gStyle->SetName(fname);
  gStyle->SetOptTitle(1);
  TCanvas* c1 = new TCanvas("Beam Spot","Beam Spot",60,40,600,800);
  c1->Divide(1,3);
  c1->SetGridx();
  c1->SetGridy();

  // beam spot at TD
  c1->cd(1);
  t1->Draw("save_y:save_x >> beamtd(60, -30., 30., 60, -30., 30.)","save_detID==890&&save_particleID==-13&&sqrt(save_x*save_x+save_y*save_y)<=40.");
  beamtd->SetTitle("Beam Spot at TD");
  beamtd->GetXaxis()->SetTitle("x (mm)");
  beamtd->GetYaxis()->SetTitle("y (mm)");
  beamtd->Draw();
  beamtd->Draw("cont0 same");

  // beam spot at sample
  c1->cd(2);
  t1->Draw("save_y:save_x >> beamsample(60, -30., 30., 60, -30., 30.)","save_detID==902&&save_particleID==-13");
  beamsample->SetTitle("Beam Spot at Sample");
  beamsample->GetXaxis()->SetTitle("x (mm)");
  beamsample->GetYaxis()->SetTitle("y (mm)");
  beamsample->Draw();
  beamsample->Draw("cont0 same");

  c1->cd(3);
  hBeamSpot->SetTitle("Beam Spot at Sample with Cut");
  hBeamSpot->GetXaxis()->SetTitle("x (mm)");
  hBeamSpot->GetYaxis()->SetTitle("y (mm)");
  hBeamSpot->Draw();
  hBeamSpot->Draw("cont0 same");
}


