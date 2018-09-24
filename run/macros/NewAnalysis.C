#define analysis_cxx
#include "analysis.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void analysis::Loop()
{
  Double_t eCut    = 0.7; //minimum energy deposition in scintillators [MeV]
  Bool_t   tofFlag = 0;
  if (fChain == 0) return;


  //DEFINE HISTOGRAMS
  TH1F* hEdeposited = new TH1F("hEdeposited","Energy spectrum; E [MeV]",        250,0.,2.5);
  TH1F* hEdepositCF = new TH1F("hEdepositCF","Energy spectrum; E [keV]",        500,0.,20.0);
  TH1F* hEdepTrig   = new TH1F("hEdepTrig",  "Radioactive electron kin. energy",250,0.,2.5);
  TH1F* hDettUp     = new TH1F("hDettUp",    "Muon arrival times Up (#mus)",    130,0.,13.);
  TH1F* hDettDown   = new TH1F("hDettDown",  "Muon arrival times Down (#mus)",  130,0.,13.);
  TH1F* hDettRight  = new TH1F("hDettRight", "Muon arrival times Right (#mus)", 130,0.,13.);
  TH1F* hDettLeft   = new TH1F("hDettLeft",  "Muon arrival times Left (#mus)",  130,0.,13.);
  TH1F* hEdepoTest  = new TH1F("hEdepoTest", "Number of events in coincidence", 100,0.,1.);
  TH1F* hTof        = new TH1F("hTof", "time-of-flight (#mus)", 1000, 0., 1.);
  TH2F* hBeamSpot   = new TH2F("hBeamSpot", " x,y", 40, -40., 40., 40, -40., 40.);
  TH1F* hDetz       = new TH1F("hDetz", "z detector [mm]", 140, -140., 140.);
  TH1F* hEventID    = new TH1F("hEventID", "Event ID", 10001, -0.5, 10000.5);
  hEdeposited->Sumw2();
  hEdepositCF->Sumw2();
  hEdepTrig->Sumw2();
  
  Long64_t nentries = fChain->GetEntriesFast();
  //nentries=1000;
  
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    tofFlag = 0;
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry); nbytes += nb;
    
    // FILL IN HISTOGRAMS
    hEdepositCF->Fill(save_ke[0]/1000.);
    hBeamSpot->Fill(save_x[0], save_y[0]);
    
    //for (Int_t i=0; i<det_n; i++)
    //  { if (det_ID[i]==623) { hEdeposited->Fill(save_ke[i]);}
    //  }
    
    hTof->Fill(muTargetTime);
    if (muTargetTime>0.23) tofFlag = 1;
    //tofFlag = 1.;
    
    // Coincidence between 1 and 33 scintillators
    for (Int_t i=0; i<det_n; i++) {
      if (det_ID[i]==1) {
	hEdeposited->Fill(det_edep[i]);
	for (Int_t j=0; j<det_n; j++){ 
	  if (det_ID[j]==36) {  
	    printf("Found coincedence\n");
	    if (det_edep[i]>eCut && det_edep[j]>eCut && tofFlag){
	      hDettUp->Fill(det_time_start[j]);
	      hDetz->Fill(det_z[i]);
	      hEventID->Fill(eventID);
	      break; //fill only once
	    }
	  }
	}
      }
    }
    
    // Coincidence between 17 and 49 scintillators
    for (Int_t i=0; i<det_n; i++) {
      if (det_ID[i]==17) {
	hEdeposited->Fill(det_edep[i]);
	for (Int_t j=0; j<det_n; j++){ 
	  if (det_ID[j]==49){ 
	    if (det_edep[i]>eCut && det_edep[j]>eCut && tofFlag){
	      hDettDown->Fill(det_time_start[j]);
	      hDetz->Fill(det_z[i]);
	      hEventID->Fill(eventID);
	      break;
	    }
	  }
	}
      }
    }
    
    // Coincidence between 9 and 42 scintillators
    for (Int_t i=0; i<det_n; i++) {
      if (det_ID[i]==9) {
	hEdeposited->Fill(det_edep[i]);
	for (Int_t j=0; j<det_n; j++){ 
	  if (det_ID[j]==42) {  
	    if (det_edep[i]>eCut && det_edep[j]>eCut && tofFlag){
	      hDettRight->Fill(det_time_start[j]);
	      hDetz->Fill(det_z[i]);
	      hEventID->Fill(eventID);
	      break;
	    }
	  } 
	}
      }
    }
      
    // Coincidence between 25 and 57 scintillators
    for (Int_t i=0; i<det_n; i++) {
      if (det_ID[i]==25) {
	hEdeposited->Fill(det_edep[i]);
	for (Int_t j=0; j<det_n; j++){ 
	  if (det_ID[j]==57){
	    if (det_edep[i]>eCut && det_edep[j]>eCut && tofFlag){
	      hDettLeft->Fill(det_time_start[j]);
	      hDetz->Fill(det_z[i]);
	      hEventID->Fill(eventID);
	      break;
	    }
	  }
	}
      }
    }        
  }

  // PLOT HISTOGRAMS
  TCanvas* c1= new TCanvas("c1","canvas 1");
  
  //   hEdeposited->Scale(1/(number_of_generated_events*hEdeposited->GetBinWidth(1))); 
  TH1F *htemp1 = (TH1F*) hDettUp->Clone(); htemp1->SetName("htemp1");
  TH1F *htemp2 = (TH1F*) hDettUp->Clone(); htemp2->SetName("htemp2");
  
  TH1F *htemp3 = (TH1F*) hDettUp->Clone(); htemp3->SetName("htemp3");
  TH1F *htemp4 = (TH1F*) hDettUp->Clone(); htemp4->SetName("htemp4");
  
  TH1F *htemp5 = (TH1F*) hDettUp->Clone(); htemp5->SetName("htemp5");
  htemp5->SetTitle("Muon decay asymmetry U-D; Time (#mus); Asymmetry");
  htemp5->Sumw2();  
  
  TH1F *htemp6 = (TH1F*) hDettUp->Clone(); htemp6->SetName("htemp6");
  htemp6->SetTitle("Muon decay asymmetry L-R; Time (#mus); Asymmetry");
  htemp6->Sumw2();
  
  htemp1->Add(hDettUp,hDettDown,1.,-1.);
  htemp2->Add(hDettUp,hDettDown,1., 1.);
  htemp5->Divide(htemp1,htemp2,1.,1.);
   
  htemp3->Add(hDettLeft,hDettRight,1.,-1.);
  htemp4->Add(hDettLeft,hDettRight,1., 1.);
  htemp6->Divide(htemp3,htemp4,1.,1.);
      
  htemp5->SetLineWidth(2);
  htemp5->SetLineColor(2);
  htemp5->SetAxisRange(-0.8,0.8,"Y");
  htemp5->Draw("hist");
   
  htemp6->SetLineWidth(2);
  htemp6->SetLineColor(4);
  htemp6->SetAxisRange(-0.8,0.8,"Y");
  htemp6->Draw("same");
   
  hEdepositCF->SetLineWidth(2);
  hEdepositCF->SetLineColor(2);
  hEdepositCF->Draw("hist");
  //hEdepositCF->Fit("gaus");
   
  TCanvas* c2= new TCanvas("c2","canvas 2");
  hEdeposited->Draw();
 
  //   sprintf(filenamePrint,"musrSrClass_%s.ps",fileNr);
  //   c1->Print(filenamePrint);
}
