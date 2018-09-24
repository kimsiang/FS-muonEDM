#define NewSpec_cxx
#include "NewSpec.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <string>
#include <TROOT.h>
#include <TKey.h>
#include <TFile.h>
#include <TSystem.h>
#include <TTree.h>

void NewSpec::CreateIO( Bool_t   FigFlag, Double_t eCut )
{
  //   Double_t eCut    = 0.7; //minimum energy deposition in scintillators [MeV]
   Bool_t   tofFlag = 0;
   if (fChain == 0) return;

   //DEFINE HISTOGRAMS
   TH1F* hEdeposited = new TH1F("hEdeposited","Energy spectrum; E [MeV]",        250,0.,2.5);
   TH1F* hEdepositCF = new TH1F("hEdepositCF","Energy spectrum; E [keV]",        500,0.,20.0);
   TH1F* hEdepTrig   = new TH1F("hEdepTrig",  "Radioactive electron kin. energy",250,0.,2.5);
   TH1F* hEdepoTest  = new TH1F("hEdepoTest", "Number of events in coincidence", 100,0.,1.);
   TH1F* hTof        = new TH1F("hTof", "time-of-flight (#mus)", 1000, 0., 1.);
   TH2F* hBeamSpot   = new TH2F("hBeamSpot", " x,y", 40, -40., 40., 40, -40., 40.);
   TH1F* hDetz       = new TH1F("hDetz", "z detector [mm]", 140, -140., 140.);
   TH1F* hEventID    = new TH1F("hEventID", "Event ID", 10001, -0.5, 10000.5);

   // Back inner histogram, i.e. all counts in segments 1-16
   TH1F* hBackI       = new TH1F("hBackI","Muon arrival times Back I (#mus)",1300,0.,13.);
   // Forward inner histogram, i.e. all counts in segments 17-32
   TH1F* hForwI       = new TH1F("hForwI","Muon arrival times Forw I (#mus)",1300,0.,13.);
   // Back outer histogram, i.e. all counts in segments 33-48
   TH1F* hBackO       = new TH1F("hBackO","Muon arrival times Back O (#mus)",1300,0.,13.);
   // Forward outer histogram, i.e. all counts in segments 49-64
   TH1F* hForwO       = new TH1F("hForwO","Muon arrival times Forw O (#mus)",1300,0.,13.);

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
      
      // Hist in Back I detector (1-16)
      for (Int_t i=0; i<det_n; i++) {
	if (det_ID[i]<=16 && det_ID[i] >=1) {
	  hEdeposited->Fill(det_edep[i]);
	  if (det_edep[i]>eCut){
	    hBackI->Fill(det_time_start[i]);
	    hDetz->Fill(det_z[i]);
	    hEventID->Fill(eventID);
	    break; //fill only once
	  }
	}
      }

      // Hist in Forw I detector (17-32)
      for (Int_t i=0; i<det_n; i++) {
	if (det_ID[i]>=17 && det_ID[i]<=32) {
	  hEdeposited->Fill(det_edep[i]);
	  if (det_edep[i]>eCut){
	    hForwI->Fill(det_time_start[i]);
	    hDetz->Fill(det_z[i]);
	    hEventID->Fill(eventID);
	    break; //fill only once
	  }
	}
      }

      // Hist in Back O detector (33-48)
      for (Int_t i=0; i<det_n; i++) {
	if (det_ID[i]<=48 && det_ID[i] >=33) {
	  //	  hEdeposited->Fill(det_edep[i]);
	  if (det_edep[i]>eCut){
	    hBackO->Fill(det_time_start[i]);
	    hDetz->Fill(det_z[i]);
	    hEventID->Fill(eventID);
	    break; //fill only once
	  }
	}
      }

      // Hist in Forw O detector (49-64)
      for (Int_t i=0; i<det_n; i++) {
	if (det_ID[i]>=49 && det_ID[i]<=64) {
	  //	  hEdeposited->Fill(det_edep[i]);
	  if (det_edep[i]>eCut){
	    hForwO->Fill(det_time_start[i]);
	    hDetz->Fill(det_z[i]);
	    hEventID->Fill(eventID);
	    break; //fill only once
	  }
	}
      }
   }

  //  Calculate Asymmetry
   // Temporary F and B histograms
   TH1F *hFtemp = (TH1F*) hForwI->Clone(); hFtemp->SetName("hFtemp");
   TH1F *hBtemp = (TH1F*) hBackI->Clone(); hBtemp->SetName("hBtemp");
   
   // Sum and difference F and B
   TH1F *hSumFB = (TH1F*) hForwI->Clone(); hSumFB->SetName("hSumFB");
   hSumFB->Sumw2();  
   TH1F *hDifFB = (TH1F*) hForwI->Clone(); hDifFB->SetName("hDifFB");
   hDifFB->Sumw2();  
   
   // Asymmetry histograms!
   TH1F *hAsyFB = (TH1F*) hForwI->Clone(); hAsyFB->SetName("hAsyFB");
   hAsyFB->SetTitle("Muon decay asymmetry F-B; Time (#mus); Asymmetry");
   hAsyFB->Sumw2();  
 
   // Calculate difference and sum, then divide
   hDifFB->Add(hFtemp,hBtemp,1.,-1.);
   hSumFB->Add(hFtemp,hBtemp,1., 1.);
   hAsyFB->Divide(hDifFB,hSumFB,1.,1.);
      
   if (FigFlag) {
     TCanvas* c1= new TCanvas("c1","canvas 1");
     c1->Divide(2,3);
     c1->cd(1);
     hBackI->Draw();
     c1->cd(2);
     hForwI->Draw();
     c1->cd(3);
     hBackO->Draw();
     c1->cd(4);
     hForwO->Draw();
     c1->cd(5);
     hAsyFB->Draw();
     hAsyFB -> Fit("pol0","Q");
     gStyle->SetOptStat(1001111);
     gStyle->SetOptFit(0001);
     gStyle->SetLineColor(2);
     c1->cd(6);
     hDetz->Draw();
   } else {
     hAsyFB -> Fit("pol0","NQ");
   }
   Double_t chi2=pol0->GetChisquare();
   Double_t p1=pol0->GetParameter(0);
   Double_t e1=pol0->GetParError(0);

   //   printf("Chi=%g\tP1=%g +/- %g\n",chi2,p1,e1);
   printf("%g\t%g\t%g\n",p1,e1,chi2);
}

void NewSpec::CoinIO( Bool_t   FigFlag, Double_t eCut )
{
  //   Double_t eCut    = 0.7; //minimum energy deposition in scintillators [MeV]
   Bool_t   tofFlag = 0;
   if (fChain == 0) return;

   //DEFINE HISTOGRAMS
   TH1F* hEdeposited = new TH1F("hEdeposited","Energy spectrum; E [MeV]",        250,0.,2.5);
   TH1F* hEdepositCF = new TH1F("hEdepositCF","Energy spectrum; E [keV]",        500,0.,20.0);
   TH1F* hEdepTrig   = new TH1F("hEdepTrig",  "Radioactive electron kin. energy",250,0.,2.5);
   TH1F* hEdepoTest  = new TH1F("hEdepoTest", "Number of events in coincidence", 100,0.,1.);
   TH1F* hTof        = new TH1F("hTof", "time-of-flight (#mus)", 1000, 0., 1.);
   TH2F* hBeamSpot   = new TH2F("hBeamSpot", " x,y", 40, -40., 40., 40, -40., 40.);
   TH1F* hDetz       = new TH1F("hDetz", "z detector [mm]", 140, -140., 140.);
   TH1F* hEventID    = new TH1F("hEventID", "Event ID", 10001, -0.5, 10000.5);
   
   // Back inner histogram, i.e. all counts in segments 1-16
   TH1F* hBack       = new TH1F("hBack","Muon arrival times Back (#mus)",1300,0.,13.);
   // Forward inner histogram, i.e. all counts in segments 17-32
   TH1F* hForw       = new TH1F("hForw","Muon arrival times Forw (#mus)",1300,0.,13.);

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
      
      // Hist in Back detector (1-16) coincidence (33-48)
      for (Int_t i=0; i<det_n; i++) {
	if (det_ID[i]<=16 && det_ID[i] >=1) {
	  hEdeposited->Fill(det_edep[i]);
	  if (det_edep[i]>eCut){
	    for (Int_t j=0; j<det_n; j++){ 
	      if (det_ID[j]<=48 && det_ID[j] >=33) {
		if (det_edep[j]>eCut){
		  hBack->Fill(det_time_start[j]);
		  hDetz->Fill(det_z[j]);
		  hEventID->Fill(eventID);
		  break; //fill only once
		}
	      }
	    }
	  }
	}
      }

      // Hist in Forw I detector (17-32) coincidence (49-64)
      for (Int_t i=0; i<det_n; i++) {
	if (det_ID[i]>=17 && det_ID[i]<=32) {
	  hEdeposited->Fill(det_edep[i]);
	  if (det_edep[i]>eCut){
	    for (Int_t j=0; j<det_n; j++){ 
	      if (det_ID[j]<=64 && det_ID[j] >=49) {
		if (det_edep[j]>eCut){
		  hForw->Fill(det_time_start[j]);
		  hDetz->Fill(det_z[j]);
		  hEventID->Fill(eventID);
		  break; //fill only once
		}
	      }
	    }
	  }
	}
      }

   }

  //  Calculate Asymmetry
   // Temporary F and B histograms
   TH1F *hFtemp = (TH1F*) hForw->Clone(); hFtemp->SetName("hFtemp");
   TH1F *hBtemp = (TH1F*) hBack->Clone(); hBtemp->SetName("hBtemp");
   
   // Sum and difference F and B
   TH1F *hSumFB = (TH1F*) hForw->Clone(); hSumFB->SetName("hSumFB");
   hSumFB->Sumw2();  
   TH1F *hDifFB = (TH1F*) hForw->Clone(); hDifFB->SetName("hDifFB");
   hDifFB->Sumw2();  
   
   // Asymmetry histograms!
   TH1F *hAsyFB = (TH1F*) hForw->Clone(); hAsyFB->SetName("hAsyFB");
   hAsyFB->SetTitle("Muon decay asymmetry F-B; Time (#mus); Asymmetry");
   hAsyFB->Sumw2();  
 
   // Calculate difference and sum, then divide
   hDifFB->Add(hFtemp,hBtemp,1.,-1.);
   hSumFB->Add(hFtemp,hBtemp,1., 1.);
   hAsyFB->Divide(hDifFB,hSumFB,1.,1.);
      
   if (FigFlag) {
     TCanvas* c1= new TCanvas("c1","canvas 1");
     c1->Divide(2,2);
     c1->cd(1);
     hBack->Draw();
     c1->cd(2);
     hForw->Draw();
     c1->cd(3);
     hAsyFB->Draw();
     hAsyFB -> Fit("pol0","Q");
     gStyle->SetOptStat(1001111);
     gStyle->SetOptFit(0001);
     gStyle->SetLineColor(2);
     c1->cd(4);
     hDetz->Draw();
   } else {
     hAsyFB -> Fit("pol0","NQ");
   }
   Double_t chi2=pol0->GetChisquare();
   Double_t p1=pol0->GetParameter(0);
   Double_t e1=pol0->GetParError(0);
   Double_t NDet=hDetz->GetSum();

   //   printf("Chi=%g\tP1=%g +/- %g\n",chi2,p1,e1);
   printf("%g\t%g\t%g\t%g\n",p1,e1,chi2,NDet);
}


void NewSpec::TBLRCoinIO( Bool_t   FigFlag, Double_t eCut )
{
  //   Double_t eCut    = 0.7; //minimum energy deposition in scintillators [MeV]
   Bool_t   tofFlag = 0;
   if (fChain == 0) return;

   //DEFINE HISTOGRAMS
   TH1F* hEdeposited = new TH1F("hEdeposited","Energy spectrum; E [MeV]",        250,0.,2.5);
   TH1F* hEdepositCF = new TH1F("hEdepositCF","Energy spectrum; E [keV]",        500,0.,20.0);
   TH1F* hEdepTrig   = new TH1F("hEdepTrig",  "Radioactive electron kin. energy",250,0.,2.5);
   TH1F* hEdepoTest  = new TH1F("hEdepoTest", "Number of events in coincidence", 100,0.,1.);
   TH1F* hTof        = new TH1F("hTof", "time-of-flight (#mus)", 1000, 0., 1.);
   TH2F* hBeamSpot   = new TH2F("hBeamSpot", " x,y", 40, -40., 40., 40, -40., 40.);
   TH1F* hDetz       = new TH1F("hDetz", "z detector [mm]", 140, -140., 140.);
   TH1F* hEventID    = new TH1F("hEventID", "Event ID", 10001, -0.5, 10000.5);
   
   // Top, Bottom, Left and Right  histograms
   TH1F* hT       = new TH1F("hT","Muon arrival times Top (#mus)",1300,0.,13.);
   TH1F* hB       = new TH1F("hB","Muon arrival times Bottom (#mus)",1300,0.,13.);
   TH1F* hL       = new TH1F("hL","Muon arrival times Left (#mus)",1300,0.,13.);
   TH1F* hR       = new TH1F("hR","Muon arrival times Right (#mus)",1300,0.,13.);

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
      
      // Hist in Top detector (11-14)+(27-30) coincidence (43-46)+(59-62)
      for (Int_t i=0; i<det_n; i++) {
	if ((det_ID[i]<=14 && det_ID[i] >=11) || (det_ID[i]<=30 && det_ID[i] >=27)) {
	  hEdeposited->Fill(det_edep[i]);
	  if (det_edep[i]>eCut){
	    for (Int_t j=0; j<det_n; j++){ 
	      if ((det_ID[j]<=46 && det_ID[j] >=43) || (det_ID[j]<=62 && det_ID[j] >=59)) {
		if (det_edep[j]>eCut){
		  hT->Fill(det_time_start[j]);
		  hDetz->Fill(det_z[j]);
		  hEventID->Fill(eventID);
		  break; //fill only once
		}
	      }
	    }
	  }
	}
      }

      // Hist in Bottom detector (3-6)+(19-22) coincidence (35-38)+(51-54)
      for (Int_t i=0; i<det_n; i++) {
	if ((det_ID[i]<=6 && det_ID[i] >=3) || (det_ID[i]<=22 && det_ID[i] >=19)) {
	  hEdeposited->Fill(det_edep[i]);
	  if (det_edep[i]>eCut){
	    for (Int_t j=0; j<det_n; j++){ 
	      if ((det_ID[j]<=38 && det_ID[j] >=35) || (det_ID[j]<=54 && det_ID[j] >=51)) {
		if (det_edep[j]>eCut){
		  hB->Fill(det_time_start[j]);
		  hDetz->Fill(det_z[j]);
		  hEventID->Fill(eventID);
		  break; //fill only once
		}
	      }
	    }
	  }
	}
      }

      // Hist in Left detector (7-10)+(23-26) coincidence (39-42)+(55-58)
      for (Int_t i=0; i<det_n; i++) {
	if ((det_ID[i]<=10 && det_ID[i] >=7) || (det_ID[i]<=26 && det_ID[i] >=23)) {
	  hEdeposited->Fill(det_edep[i]);
	  if (det_edep[i]>eCut){
	    for (Int_t j=0; j<det_n; j++){ 
	      if ((det_ID[j]<=42 && det_ID[j] >=39) || (det_ID[j]<=58 && det_ID[j] >=55)) {
		if (det_edep[j]>eCut){
		  hL->Fill(det_time_start[j]);
		  hDetz->Fill(det_z[j]);
		  hEventID->Fill(eventID);
		  break; //fill only once
		}
	      }
	    }
	  }
	}
      }

      // Hist in Right detector (1,2,15,16)+(17,18,31,32) coincidence (33,34,47,48)+(49,50,63,64)
      for (Int_t i=0; i<det_n; i++) {
	if ((det_ID[i]<=18 && det_ID[i] >=15) || (det_ID[i]<=2 && det_ID[i] >=1) || (det_ID[i]<=32 && det_ID[i] >=31)) {
	  hEdeposited->Fill(det_edep[i]);
	  if (det_edep[i]>eCut){
	    for (Int_t j=0; j<det_n; j++){ 
	      if ((det_ID[j]<=50 && det_ID[j] >=47) || (det_ID[j]<=34 && det_ID[j] >=33) || (det_ID[j]<=64 && det_ID[j] >=63)) {
		if (det_edep[j]>eCut){
		  hR->Fill(det_time_start[j]);
		  hDetz->Fill(det_z[j]);
		  hEventID->Fill(eventID);
		  break; //fill only once
		}
	      }
	    }
	  }
	}
      }




   }

  //  Calculate T-B Asymmetry
   // Temporary T and B histograms
   TH1F *hTtemp = (TH1F*) hT->Clone(); hTtemp->SetName("hTtemp");
   TH1F *hBtemp = (TH1F*) hB->Clone(); hBtemp->SetName("hBtemp");
   
   // Sum and difference T and B
   TH1F *hSumTB = (TH1F*) hT->Clone(); hSumTB->SetName("hSumTB");
   hSumTB->Sumw2();  
   TH1F *hDifTB = (TH1F*) hT->Clone(); hDifTB->SetName("hDifTB");
   hDifTB->Sumw2();  
   
   // Asymmetry histograms!
   TH1F *hAsyTB = (TH1F*) hT->Clone(); hAsyTB->SetName("hAsyTB");
   hAsyTB->SetTitle("Muon decay asymmetry T-B; Time (#mus); Asymmetry");
   hAsyTB->Sumw2();  
 
   // Calculate difference and sum, then divide
   hDifTB->Add(hTtemp,hBtemp,1.,-1.);
   hSumTB->Add(hTtemp,hBtemp,1., 1.);
   hAsyTB->Divide(hDifTB,hSumTB,1.,1.);
      
  //  Calculate L-R Asymmetry
   // Temporary L and R histograms
   TH1F *hLtemp = (TH1F*) hL->Clone(); hLtemp->SetName("hLtemp");
   TH1F *hRtemp = (TH1F*) hR->Clone(); hRtemp->SetName("hRtemp");
   
   // Sum and difference L and R
   TH1F *hSumLR = (TH1F*) hL->Clone(); hSumLR->SetName("hSumLR");
   hSumLR->Sumw2();  
   TH1F *hDifLR = (TH1F*) hL->Clone(); hDifLR->SetName("hDifLR");
   hDifLR->Sumw2();  
   
   // Asymmetry histograms!
   TH1F *hAsyLR = (TH1F*) hL->Clone(); hAsyLR->SetName("hAsyLR");
   hAsyLR->SetTitle("Muon decay asymmetry L-R; Time (#mus); Asymmetry");
   hAsyLR->Sumw2();  
 
   // Calculate difference and sum, then divide
   hDifLR->Add(hLtemp,hRtemp,1.,-1.);
   hSumLR->Add(hLtemp,hRtemp,1., 1.);
   hAsyLR->Divide(hDifLR,hSumLR,1.,1.);

   if (FigFlag) {
     // Top - Bottom
     TCanvas* c1= new TCanvas("c1","canvas 1");
     c1->Divide(2,2);
     c1->cd(1);
     hT->Draw();
     c1->cd(2);
     hB->Draw();
     c1->cd(3);
     hAsyTB->Draw();
     hAsyTB -> Fit("pol0","Q");
     // Top - Bottom
     Double_t TBchi2=pol0->GetChisquare();
     Double_t TBp1=pol0->GetParameter(0);
     Double_t TBe1=pol0->GetParError(0);
     gStyle->SetOptStat(1001111);
     gStyle->SetOptFit(0001);
     gStyle->SetLineColor(2);
     c1->cd(4);
     hDetz->Draw();

     // Left - Right
     TCanvas* c2= new TCanvas("c2","canvas 2");
     c2->Divide(2,2);
     c2->cd(1);
     hL->Draw();
     c2->cd(2);
     hR->Draw();
     c2->cd(3);
     hAsyLR->Draw();
     hAsyLR -> Fit("pol0","Q");
     // Left - Right
     Double_t LRchi2=pol0->GetChisquare();
     Double_t LRp1=pol0->GetParameter(0);
     Double_t LRe1=pol0->GetParError(0);
     gStyle->SetOptStat(1001111);
     gStyle->SetOptFit(0001);
     gStyle->SetLineColor(2);
     c2->cd(4);
     hDetz->Draw();
   } else {
     hAsyTB -> Fit("pol0","NQ");
     // Top - Bottom
     Double_t TBchi2=pol0->GetChisquare();
     Double_t TBp1=pol0->GetParameter(0);
     Double_t TBe1=pol0->GetParError(0);
     
     hAsyLR -> Fit("pol0","NQ");
     // Left - Right
     Double_t LRchi2=pol0->GetChisquare();
     Double_t LRp1=pol0->GetParameter(0);
     Double_t LRe1=pol0->GetParError(0);
   }

   Double_t NDet=hDetz->GetSum();

   //   printf("Chi=%g\tP1=%g +/- %g\n",chi2,p1,e1);
   printf("%g\t%g\t%g\t%g\t%g\n",TBp1,TBe1,LRp1,LRe1,NDet);
}


void NewSpec::SCoinIO( Bool_t   FigFlag, Double_t eCut )
{
  //   Double_t eCut    = 0.7; //minimum energy deposition in scintillators [MeV]
   Bool_t   tofFlag = 0;
   if (fChain == 0) return;

   //DEFINE HISTOGRAMS
   TH1F* hEdeposited = new TH1F("hEdeposited","Energy spectrum; E [MeV]",        250,0.,2.5);
   TH1F* hEdepositCF = new TH1F("hEdepositCF","Energy spectrum; E [keV]",        500,0.,20.0);
   TH1F* hEdepTrig   = new TH1F("hEdepTrig",  "Radioactive electron kin. energy",250,0.,2.5);
   TH1F* hEdepoTest  = new TH1F("hEdepoTest", "Number of events in coincidence", 100,0.,1.);
   TH1F* hTof        = new TH1F("hTof", "time-of-flight (#mus)", 1000, 0., 1.);
   TH2F* hBeamSpot   = new TH2F("hBeamSpot", " x,y", 40, -40., 40., 40, -40., 40.);
   TH1F* hDetz       = new TH1F("hDetz", "z detector [mm]", 140, -140., 140.);
   TH1F* hEventID    = new TH1F("hEventID", "Event ID", 10001, -0.5, 10000.5);
   
   // Sigmenets histograms 1-32
   TH1F* h1       = new TH1F("h1","Muon arrival times in 1 (#mus)",1300,0.,13.);

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
      
      // Segments (1-16) coincidence (33-48)
      for (Int_t i=0; i<det_n; i++) {
	if (det_ID[i]==1) {
	  hEdeposited->Fill(det_edep[i]);
	  if (det_edep[i]>eCut){
	    for (Int_t j=0; j<det_n; j++){ 
	      if (det_ID[j]==33) {
		if (det_edep[j]>eCut){
		  h1->Fill(det_time_start[j]);
		  hDetz->Fill(det_z[j]);
		  hEventID->Fill(eventID);
		  break; //fill only once
		}
	      }
	    }
	  }
	}
      }

   }

  //  Calculate Asymmetry
   // Temporary F and B histograms
   TH1F *hFtemp = (TH1F*) hForw->Clone(); hFtemp->SetName("hFtemp");
   TH1F *hBtemp = (TH1F*) hBack->Clone(); hBtemp->SetName("hBtemp");
   
   // Sum and difference F and B
   TH1F *hSumFB = (TH1F*) hForw->Clone(); hSumFB->SetName("hSumFB");
   hSumFB->Sumw2();  
   TH1F *hDifFB = (TH1F*) hForw->Clone(); hDifFB->SetName("hDifFB");
   hDifFB->Sumw2();  
   
   // Asymmetry histograms!
   TH1F *hAsyFB = (TH1F*) hForw->Clone(); hAsyFB->SetName("hAsyFB");
   hAsyFB->SetTitle("Muon decay asymmetry F-B; Time (#mus); Asymmetry");
   hAsyFB->Sumw2();  
 
   // Calculate difference and sum, then divide
   hDifFB->Add(hFtemp,hBtemp,1.,-1.);
   hSumFB->Add(hFtemp,hBtemp,1., 1.);
   hAsyFB->Divide(hDifFB,hSumFB,1.,1.);
      
   if (FigFlag) {
     TCanvas* c1= new TCanvas("c1","canvas 1");
     c1->Divide(2,2);
     c1->cd(1);
     hBack->Draw();
     c1->cd(2);
     hForw->Draw();
     c1->cd(3);
     hAsyFB->Draw();
     hAsyFB -> Fit("pol0","Q");
     gStyle->SetOptStat(1001111);
     gStyle->SetOptFit(0001);
     gStyle->SetLineColor(2);
     c1->cd(4);
     hDetz->Draw();
   } else {
     hAsyFB -> Fit("pol0","NQ");
   }
   Double_t chi2=pol0->GetChisquare();
   Double_t p1=pol0->GetParameter(0);
   Double_t e1=pol0->GetParError(0);
   Double_t NDet=hDetz->GetSum();

   //   printf("Chi=%g\tP1=%g +/- %g\n",chi2,p1,e1);
   printf("%g\t%g\t%g\t%g\n",p1,e1,chi2,NDet);
}
