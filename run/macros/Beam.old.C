#define Beam_cxx
#include "Beam.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void Beam::Loop()
{
   if (fChain == 0) return;

   gROOT->SetStyle("Plain");
   //   gStyle->SetTitleBorderSize(0);
   gStyle->SetTitleFontSize(0.06);
   //   gStyle->SetLabelSize(0.055);   gStyle->SetTitleXSize(0.055);   gStyle->SetTitleYSize(0.055);
   //   gStyle->SetTitleXOffset(0.8);  // OK for the beam profiles
   gStyle->SetTitleXSize(0.046);   gStyle->SetTitleYSize(0.046);  gStyle->SetTitleXOffset(0.9);
   gStyle->SetGridWidth(0.5);
   const Double_t pi=3.14159265358979324;
   color1=red; color2=blue; color3=green; color4=lightblue;
   Double_t R_collim1=16.5;
   Double_t R_collim2=16.5;
   Double_t sampleRadius=2.5;
   Int_t save_detID_firstplain=401;
   Int_t save_detID_sample=601;
   Int_t    save_detID_coll2=536;  //for 101051-3
   //   Int_t    save_detID_coll2=581;
   char filenamePrint[1000];
   char cLabel[1000];
   char cLabel2[1000];
   char colimExtention[100];
   //   char colimExtention[]="_Colim10";
   //   char colimExtention[]="_coll_16mm";
   //   char colimExtention[]="_coll_33mm";

   char volba[]="101072_coll16mm";
   //   char volba[]="101071_coll16mm";
   //   char volba[]="101053_coll16mm";
   //   char volba[]="101062_coll33mm";
   //char volba[]="101062_coll16mm";
   //   char volba[]="movedBy20cm";
   if (strcmp(volba,"101072_coll16mm")==0) {
     R_collim1=8.25;
     R_collim2=8.25;
     save_detID_coll2=536;
     strcpy(cLabel2,"#splitline{}{R_{coll 1} = R_{coll 2} = 8.25mm}");
     strcpy(colimExtention,"_coll_16mm");
   }   
   if (strcmp(volba,"101071_coll16mm")==0) {
     R_collim1=8.25;
     R_collim2=8.25;
     save_detID_coll2=536;
     strcpy(cLabel2,"#splitline{}{R_{coll 1} = R_{coll 2} = 8.25mm}");
     strcpy(colimExtention,"_coll_16mm");
   }
   if (strcmp(volba,"101053_coll16mm")==0) {
     R_collim1=8.25;
     R_collim2=8.25;
     save_detID_coll2=536;
     strcpy(cLabel2,"#splitline{}{R_{coll 1} = R_{coll 2} = 8.25mm}");
     strcpy(colimExtention,"_coll_16mm");
   }
   if (strcmp(volba,"101053_coll33mm")==0) {
     R_collim1=16.5;
     R_collim2=16.5;
     save_detID_coll2=536;
     strcpy(cLabel2,"#splitline{}{R_{coll 1} = R_{coll 2} = 16.5mm}");
     strcpy(colimExtention,"_coll_33mm");
   }
   if (strcmp(volba,"101062_coll33mm")==0) {
     R_collim1=16.5;
     R_collim2=16.5;
     save_detID_coll2=1336;
     strcpy(cLabel2,"#splitline{}{R_{coll 1} = R_{coll 2} = 16.5mm}");
     strcpy(colimExtention,"_coll_33mm");
   }
   if (strcmp(volba,"101062_coll16mm")==0) {
     R_collim1=8.25;
     R_collim2=8.25;
     save_detID_coll2=1336;
     strcpy(cLabel2,"#splitline{}{R_{coll 1} = R_{coll 2} = 8.25mm}");
     strcpy(colimExtention,"_coll_16mm");
   }
   if (strcmp(volba,"movedBy10cm")==0) {
     R_collim1=8.25;
     R_collim2=8.25;
     save_detID_sample=1381;
     save_detID_coll2=1316;
     strcpy(cLabel2,"#splitline{Sample moved to z=-10cm, coll_{2} to z=-42.5cm}{R_{coll 1} = R_{coll 2} = 8.25mm (Only 0 field makes any sence.)}");
     strcpy(colimExtention,"_coll_16mm_focus100mm");
   }
   if (strcmp(volba,"movedBy20cm")==0) {
     R_collim1=8.25;
     R_collim2=8.25;
     save_detID_sample=1361;
     save_detID_coll2=1296;
     strcpy(cLabel2,"#splitline{Sample moved to z=-20cm, coll_{2} to z=-52.5cm}{R_{coll 1} = R_{coll 2} = 8.25mm (Only 0 field makes any sence.)}");
     strcpy(colimExtention,"_coll_16mm_focus200mm");
   }


   if      (strcmp(fileNr,"100040")==0) {strcpy(cLabel,"ALC magnet; like 100411.mac");}
   else if (strcmp(fileNr,"100111_4")==0) {strcpy(cLabel,"M0=0mm,  FWHM(momentum spread)=0%");}
   else if (strcmp(fileNr,"100121_4")==0) {strcpy(cLabel,"M0=0mm,  FWHM(momentum spread)=3%");}
   else if (strcmp(fileNr,"100131_4")==0) {strcpy(cLabel,"M0=0.4mm,  FWHM(momentum spread)=0%, l=20mm");}
   else if (strcmp(fileNr,"100141_4")==0) {strcpy(cLabel,"M0=0.4mm,  FWHM(momentum spread)=3%, l=20mm");}
   else if (strcmp(fileNr,"100151_4")==0) {strcpy(cLabel,"M0=0.4mm,  FWHM(momentum spread)=3%, l=5mm");}
   else if (strcmp(fileNr,"100161")==0) {strcpy(cLabel,"M0=0.4mm,  FWHM(P)=3%, l=20mm; gaussian beam");}
   else if (strcmp(fileNr,"100162")==0) {strcpy(cLabel,"M0=0.4mm,  FWHM(P)=3%, l=20mm; circular beam");}
   else if (strcmp(fileNr,"100171_4")==0) {strcpy(cLabel,"M0=0.4mm,  FWHM(P)=3%, l=20mm, CRYOGENICS");}
   else if (strcmp(fileNr,"100311")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=3%, l=20mm; Gauss. parall. beam");}
   else if (strcmp(fileNr,"100312")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=8%, l=20mm; Gauss. parall. beam");}
   else if (strcmp(fileNr,"100313")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=3%, l=20mm; beam focused to sample");}
   else if (strcmp(fileNr,"100314")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=3%, l=20mm; beam foc. 5cm before sample");}
   else if (strcmp(fileNr,"100315")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=3%, l=20mm; beam foc. 20cm before sample");}
   else if (strcmp(fileNr,"100316")==0) {strcpy(cLabel,"M0=0.4mm, l=20mm; TURTLE as is");}
   else if (strcmp(fileNr,"100317")==0) {strcpy(cLabel,"M0=0.4mm, l=20mm; TURTLE with pitch=-0.018 deg");}
   else if (strcmp(fileNr,"100318")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=3%, l=20mm; beam foc. 50cm after sample");}
   else if (strcmp(fileNr,"100319")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=3%, l=20mm; beam foc. 10cm after sample");}
   else if (strcmp(fileNr,"100321")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=3%, l=20mm; Gauss. parall. beam");}
   else if (strcmp(fileNr,"100322")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=8%, l=20mm; Gauss. parall. beam");}
   else if (strcmp(fileNr,"100323")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=3%, l=20mm; beam focused to sample");}
   else if (strcmp(fileNr,"100324")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=3%, l=20mm; beam foc. 5cm bef. sample");}
   else if (strcmp(fileNr,"100325")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=3%, l=20mm; beam foc. 20cm bef. sample");}
   else if (strcmp(fileNr,"100326")==0) {strcpy(cLabel,"M0=0.4mm, l=20mm; TURTLE as is");}
   else if (strcmp(fileNr,"100327")==0) {strcpy(cLabel,"M0=0.4mm, l=20mm; TURTLE with pitch=-0.018 deg");}
   else if (strcmp(fileNr,"100328")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=3%, l=20mm; beam foc. 50cm after sample");}
   else if (strcmp(fileNr,"100329")==0) {strcpy(cLabel,"M0=0.4mm, FWHM(P)=3%, l=20mm; beam foc. 10cm after sample");}
   else if (strcmp(fileNr,"100331")==0) {strcpy(cLabel,"M0=0.4mm, l=20mm, TURTLE as is, Collim r=10mm");}
   else if (strcmp(fileNr,"100332")==0) {strcpy(cLabel,"M0=0.4mm, l=20mm, TURTLE as is, Collim r=7.5mm");}
   else if (strcmp(fileNr,"100333")==0) {strcpy(cLabel,"M0=0.4mm, l=20mm, TURTLE as is, Collim r=5mm");}
   else if (strcmp(fileNr,"100341")==0) {strcpy(cLabel,"M0=0.4mm, l=20mm, TURTLE with pitch, Collim r=10mm");}
   else if (strcmp(fileNr,"100342")==0) {strcpy(cLabel,"M0=0.4mm, l=20mm, TURTLE with pitch, Collim r=7.5mm");}
   else if (strcmp(fileNr,"100343")==0) {strcpy(cLabel,"M0=0.4mm, l=20mm, TURTLE with pitch, Collim r=5mm");}
   else if (strcmp(fileNr,"100381")==0) {strcpy(cLabel,"M0=0.4mm, l=20mm; TURTLE with pitch=-0.018 deg repeated");}
   else if (strcmp(fileNr,"100384")==0) {strcpy(cLabel,"100384: No detectors, TURTLE as is");}
   else if (strcmp(fileNr,"100385")==0) {strcpy(cLabel,"100385: No detectors, TURTLE with pitch=-0.018 deg");}
   else if (strcmp(fileNr,"100386")==0) {strcpy(cLabel,"100386: No detectors, Gauss. focused beam ");}
   else if (strcmp(fileNr,"100387")==0) {strcpy(cLabel,"100387: No detectors, Gauss. parallel beam ");}
   else if (strcmp(fileNr,"100388")==0) {strcpy(cLabel,"100388: No detectors, Gauss. par. beam (small sigma in y)");}
   else if (strcmp(fileNr,"100389")==0) {strcpy(cLabel,"100389: No detectors, Gauss. par. beam (offset in y=5mm)");}
   else if (strcmp(fileNr,"100390")==0) {strcpy(cLabel,"100390: No detectors, G. p. beam (tilt in y corresp. 5mm)");}
   else if (strcmp(fileNr,"100392")==0) {strcpy(cLabel,"100392: No detectors, New Turtle (61)");}
   else if (strcmp(fileNr,"100393")==0) {strcpy(cLabel,"100393: No detectors, New Turtle (70)");}
   else if (strcmp(fileNr,"100395")==0) {strcpy(cLabel,"100395: No detectors, Gaussian beam (large #sigma)");}
   else if (strcmp(fileNr,"100396")==0) {strcpy(cLabel,"100396: TURTLE, M0 Scint. plate 0.4mm at -15cm");}
   else if (strcmp(fileNr,"100397")==0) {strcpy(cLabel,"100397 (repete of 100392): No detectors, New Turtle (61)");}
   //
   else if (strcmp(fileNr,"100411")==0) {strcpy(cLabel,"CRG: FWHM(P)=3%, l=20mm; Gauss. parall. beam");}
   else if (strcmp(fileNr,"100412")==0) {strcpy(cLabel,"CRG: FWHM(P)=8%, l=20mm; Gauss. parall. beam");}
   else if (strcmp(fileNr,"100413")==0) {strcpy(cLabel,"CRG: FWHM(P)=3%, l=20mm; beam focused to sample");}
   else if (strcmp(fileNr,"100414")==0) {strcpy(cLabel,"CRG: FWHM(P)=3%, l=20mm; beam foc. 5cm before sample");}
   else if (strcmp(fileNr,"100421")==0) {strcpy(cLabel,"CRG: FWHM(P)=3%, l=20mm; Gauss. parall. beam");}
   else if (strcmp(fileNr,"100422")==0) {strcpy(cLabel,"CRG: FWHM(P)=8%, l=20mm; Gauss. parall. beam");}
   else if (strcmp(fileNr,"100423")==0) {strcpy(cLabel,"CRG: FWHM(P)=3%, l=20mm; beam focused to sample");}
   else if (strcmp(fileNr,"100424")==0) {strcpy(cLabel,"CRG: FWHM(P)=3%, l=20mm; beam foc. 5cm bef. sample");}
   //
   else if (strcmp(fileNr,"100501")==0) {strcpy(cLabel,"100501: M0=0.4mm, l=20mm; TURTLE(61)");}
   else if (strcmp(fileNr,"100502")==0) {strcpy(cLabel,"100502: M0=0.4mm, l=20mm; TURTLE(61), R_{coll.}=10mm");}
   else if (strcmp(fileNr,"100504")==0) {strcpy(cLabel,"100504: M0=0.4mm, l=20mm; TURTLE(61), R_{coll.}=5mm");}
   //
   else if (strcmp(fileNr,"100601")==0) {strcpy(cLabel,"100601: M0=0.4mm, l=20mm; TURTLE(61)");}
   else if (strcmp(fileNr,"100602")==0) {strcpy(cLabel,"100602: M0=0.4mm, l=20mm; TURTLE(61), R_{coll.}=10mm");}
   else if (strcmp(fileNr,"100604")==0) {strcpy(cLabel,"100604: M0=0.4mm, l=20mm; TURTLE(61), R_{coll.}=5mm");}
   
   else if (strcmp(fileNr,"101002")==0) {strcpy(cLabel,"101002: No detectors, New Turtle (61), GlobalField");}
   else if (strcmp(fileNr,"101005")==0) {strcpy(cLabel,"101005: No detectors, Quadrupole");}
   else if (strcmp(fileNr,"101006")==0) {strcpy(cLabel,"101006: parralel beam");}
   else if (strcmp(fileNr,"101007")==0) {strcpy(cLabel,"101007: focused to centre");}
   else if (strcmp(fileNr,"101008")==0) {strcpy(cLabel,"101008: focused to -11cm");}
   else if (strcmp(fileNr,"101009")==0) {strcpy(cLabel,"101009: focused to -3cm");}
   else if (strcmp(fileNr,"101010")==0) {strcpy(cLabel,"101010: focused to -21cm");}
   else if (strcmp(fileNr,"101011")==0) {strcpy(cLabel,"101011: ALC quadrupoles (as real)");}
   else if (strcmp(fileNr,"101013")==0) {strcpy(cLabel,"101013: quads + Turtle60");}  // some muons out of the magnet aperture)
   else if (strcmp(fileNr,"101014")==0) {strcpy(cLabel,"101014: Turtle61");}
   else if (strcmp(fileNr,"101018")==0) {strcpy(cLabel,"101018: quads + parralel beam");}
   else if (strcmp(fileNr,"101019")==0) {strcpy(cLabel,"101019: quads + Turtle60");}
   else if (strcmp(fileNr,"101021")==0) {strcpy(cLabel,"101021: F=10cm");}
   else if (strcmp(fileNr,"101022")==0) {strcpy(cLabel,"101022: F=10cm;C1=-80cm");}
   else if (strcmp(fileNr,"101023")==0) {strcpy(cLabel,"101023: F=10cm;C1=-80cm;C2=-10cm");}
   else if (strcmp(fileNr,"101031")==0) {strcpy(cLabel,"101031: F=-20cm");}
   else if (strcmp(fileNr,"101032")==0) {strcpy(cLabel,"101032: F=-20cm;C1=-80cm");}
   else if (strcmp(fileNr,"101033")==0) {strcpy(cLabel,"101033: F=-20cm;C1=-80cm;C2=-10cm");}
   else if (strcmp(fileNr,"101051")==0) {strcpy(cLabel,"101051: F=10cm");}
   else if (strcmp(fileNr,"101052")==0) {strcpy(cLabel,"101052: F=10cm");}
   else if (strcmp(fileNr,"101053")==0) {strcpy(cLabel,"101053: 1m beampipe; coll2=-32.5cm");}
   else if (strcmp(fileNr,"101061")==0) {strcpy(cLabel,"101061: F=??cm; 2m beampipe");}
   else if (strcmp(fileNr,"101062")==0) {strcpy(cLabel,"101062: 2m beampipe; coll2=-32.5cm");}
   else if (strcmp(fileNr,"101071")==0) {strcpy(cLabel,"101071: 1m beampipe; coll2=-32.5cm");}
   else if (strcmp(fileNr,"101072")==0) {strcpy(cLabel,"101072: 1m beampipe; coll2=-32.5cm");}

   //   sprintf(filename,"data/musr_%s.root",fileNr);  //100111_4.mac
   //   TFile* f=new TFile(filename);

   TCanvas* c1= new TCanvas("c1","canvas 1");
   TCanvas* c2= new TCanvas("c2","canvas 2");
   TCanvas* c3= new TCanvas("c3","canvas 3");
   c3->Divide(2,2);
   //   TCanvas* c4= new TCanvas("c4","canvas 4");
   //   c4->GetFrame()->SetFillColor(10);
   //   c4->SetFillColor(10);
   //   c4->Divide(3,4,0,0);
   TCanvas* c5= new TCanvas("c5","canvas 5");
   c5->SetFillColor(10);
   c5->Divide(3,4,0,0);
   TCanvas* c6= new TCanvas("c6","canvas 6");
   c6->SetFillColor(10);
   c6->Divide(3,4,0,0);
   TCanvas* c7= new TCanvas("c7","canvas 7");
   c7->Divide(2,2);
   TCanvas* c8= new TCanvas("c8","canvas 8");
   c8->Divide(3,4,0,0);
   //   c4->GetFrame()->SetFillColor(10);
   TCanvas* c9= new TCanvas("c9","canvas 9");
   c9->Divide(3,4,0,0);
   TCanvas* c10= new TCanvas("c10","canvas 10");
   c10->Divide(3,4,0,0);


   TH2F* hBeamR_T = new TH2F("hBeamR_T","Beam R at Target;B [T];R [mm]",1001,-0.005,10.005,100,0,25);
   hBeamR_T->Clone("hBeamR_T_1"); hBeamR_T_1->SetLineColor(color1); hBeamR_T_1->SetMarkerColor(color1);
   hBeamR_T->Clone("hBeamR_T_2"); hBeamR_T_2->SetLineColor(color2); hBeamR_T_2->SetMarkerColor(color2);
   hBeamR_T->Clone("hBeamR_T_3"); hBeamR_T_3->SetLineColor(color3); hBeamR_T_3->SetMarkerColor(color3);
   hBeamR_T->Clone("hBeamR_T_4"); hBeamR_T_4->SetLineColor(color4); hBeamR_T_4->SetMarkerColor(color4);

   TProfile* hBeamR_T_prof = new TProfile("hBeamR_T_prof","Beam R at Target;B [T];R [mm]",1001,-0.005,10.005,"s");
   hBeamR_T_prof->Clone("hBeamR_T_prof_1");hBeamR_T_prof_1->SetLineColor(color1); hBeamR_T_prof_1->SetMarkerColor(color1);
   hBeamR_T_prof->Clone("hBeamR_T_prof_2");hBeamR_T_prof_2->SetLineColor(color2); hBeamR_T_prof_2->SetMarkerColor(color2);
   hBeamR_T_prof->Clone("hBeamR_T_prof_3");hBeamR_T_prof_3->SetLineColor(color3); hBeamR_T_prof_3->SetMarkerColor(color3);
   hBeamR_T_prof->Clone("hBeamR_T_prof_4");hBeamR_T_prof_4->SetLineColor(color4); hBeamR_T_prof_4->SetMarkerColor(color4);


   TH2F* hBeamR_M0 = new TH2F("hBeamR_M0","Beam R at M0;B [T];R [mm]",1001,-0.005,10.005,100,0,25);
   hBeamR_M0->Clone("hBeamR_M0_1");hBeamR_M0_1->SetLineColor(color1); hBeamR_M0_1->SetMarkerColor(color1);
   hBeamR_M0->Clone("hBeamR_M0_2");hBeamR_M0_2->SetLineColor(color2); hBeamR_M0_2->SetMarkerColor(color2);
   hBeamR_M0->Clone("hBeamR_M0_3");hBeamR_M0_3->SetLineColor(color3); hBeamR_M0_3->SetMarkerColor(color3);
   hBeamR_M0->Clone("hBeamR_M0_4");hBeamR_M0_4->SetLineColor(color4); hBeamR_M0_4->SetMarkerColor(color4);


   TProfile* hBeamR_M0_prof = new TProfile("hBeamR_M0_prof","Beam R at M0;B [T];R [mm]",1001,-0.005,10.005,"s");
   hBeamR_M0_prof->Clone("hBeamR_M0_prof_1");hBeamR_M0_prof_1->SetLineColor(color1); hBeamR_M0_prof_1->SetMarkerColor(color1);
   hBeamR_M0_prof->Clone("hBeamR_M0_prof_2");hBeamR_M0_prof_2->SetLineColor(color2); hBeamR_M0_prof_2->SetMarkerColor(color2);
   hBeamR_M0_prof->Clone("hBeamR_M0_prof_3");hBeamR_M0_prof_3->SetLineColor(color3); hBeamR_M0_prof_3->SetMarkerColor(color3);
   hBeamR_M0_prof->Clone("hBeamR_M0_prof_4");hBeamR_M0_prof_4->SetLineColor(color4); hBeamR_M0_prof_4->SetMarkerColor(color4);


   TH2F* hMuonPolPhi = new TH2F("hMuonPolPhi","#phi angle of the muon spin when entering the sample;B [T];P#phi;N",1001,-0.005,10.005,50,-pi,2*pi);
   hMuonPolPhi->Clone("hMuonPolPhi_1");hMuonPolPhi_1->SetLineColor(color1); hMuonPolPhi_1->SetMarkerColor(color1);
   hMuonPolPhi->Clone("hMuonPolPhi_2");hMuonPolPhi_2->SetLineColor(color2); hMuonPolPhi_2->SetMarkerColor(color2);
   hMuonPolPhi->Clone("hMuonPolPhi_3");hMuonPolPhi_3->SetLineColor(color3); hMuonPolPhi_3->SetMarkerColor(color3);
   hMuonPolPhi->Clone("hMuonPolPhi_4");hMuonPolPhi_4->SetLineColor(color4); hMuonPolPhi_4->SetMarkerColor(color4);


   TProfile* hMuonPolPhi_prof = new TProfile("hMuonPolPhi_prof","#phi angle of the muon spin when entering the sample;B [T];P#phi",1001,-0.005,10.005,"s");
   hMuonPolPhi_prof->Clone("hMuonPolPhi_prof_1");hMuonPolPhi_prof_1->SetLineColor(color1); hMuonPolPhi_prof_1->SetMarkerColor(color1);
   hMuonPolPhi_prof->Clone("hMuonPolPhi_prof_2");hMuonPolPhi_prof_2->SetLineColor(color2); hMuonPolPhi_prof_2->SetMarkerColor(color2);
   hMuonPolPhi_prof->Clone("hMuonPolPhi_prof_3");hMuonPolPhi_prof_3->SetLineColor(color3); hMuonPolPhi_prof_3->SetMarkerColor(color3);
   hMuonPolPhi_prof->Clone("hMuonPolPhi_prof_4");hMuonPolPhi_prof_4->SetLineColor(color4); hMuonPolPhi_prof_4->SetMarkerColor(color4);


   TH2F* hMuonPolR = new TH2F("hMuonPolR","Transverse polarisation at arival to sample;B [T];|P_{T}|",1001,-0.005,10.005,110,0.92,1.01);
   hMuonPolR->Clone("hMuonPolR_1"); hMuonPolR_1->SetLineColor(color1);  hMuonPolR_1->SetMarkerColor(color1);
   hMuonPolR->Clone("hMuonPolR_2"); hMuonPolR_2->SetLineColor(color2);  hMuonPolR_2->SetMarkerColor(color2);
   hMuonPolR->Clone("hMuonPolR_3"); hMuonPolR_3->SetLineColor(color3);  hMuonPolR_3->SetMarkerColor(color3);
   hMuonPolR->Clone("hMuonPolR_4"); hMuonPolR_4->SetLineColor(color4);  hMuonPolR_4->SetMarkerColor(color4);


   TProfile* hMuonPolR_prof = new TProfile("hMuonPolR_prof","Transverse polarisation at arival to sample;B [T];|P_{T}|",1001,-0.005,10.005);
   hMuonPolR_prof->Clone("hMuonPolR_prof_1"); hMuonPolR_prof_1->SetLineColor(color1);  hMuonPolR_prof_1->SetMarkerColor(color1);
   hMuonPolR_prof->Clone("hMuonPolR_prof_2"); hMuonPolR_prof_2->SetLineColor(color2);  hMuonPolR_prof_2->SetMarkerColor(color2);
   hMuonPolR_prof->Clone("hMuonPolR_prof_3"); hMuonPolR_prof_3->SetLineColor(color3);  hMuonPolR_prof_3->SetMarkerColor(color3);
   hMuonPolR_prof->Clone("hMuonPolR_prof_4"); hMuonPolR_prof_4->SetLineColor(color4);  hMuonPolR_prof_4->SetMarkerColor(color4);

   TH1F* hEff0 = new TH1F("hEff0","All events (for normalisation purposes)",101,-0.05,10.05);
   TH1F* hEff1 = new TH1F("hEff1","M0 and Sample",101,-0.05,10.05);
   TH1F* hEff2 = new TH1F("hEff2","M0 and not Sample",101,-0.05,10.05);
   TH1F* hEff3 = new TH1F("hEff3","not M0 and Sample",101,-0.05,10.05);
   hEff1->Clone("hEff1_Norm"); hEff1->SetTitle("M0 && Sample;B [T];efficiency");
   hEff2->Clone("hEff2_Norm"); hEff2->SetTitle("M0 && !Sample;B [T];efficiency");
   hEff3->Clone("hEff3_Norm"); hEff3->SetTitle("!M0 && Sample;B [T];efficiency");
   hEff0->Sumw2();   hEff1->Sumw2();   hEff2->Sumw2();   hEff3->Sumw2();


   TH1F* beamHistArrayX[12];
   TH1F* beamHistArrayY[12];
   TH1F* bHistArrayX[12];
   TH1F* bHistArrayY[12];
   TH1F* beamCollArrayX[12];
   TH1F* beamCollArrayY[12];
   TH1F* bCollArrayX[12];
   TH1F* bCollArrayY[12];
   TH1F* beamTransmission[12];
   TH1F* bTransmission[12];
   TH1F* beamSample[12];
   TH1F* bSample[12];
   TH1F* beamSampleMap[12];
   TH1F* bSampleMap[12];
   TProfile* beamProfileHistArrayX[12];
   TProfile* beamProfileHistArrayY[12];
   TProfile* bProfileHistArrayX[12];
   TProfile* bProfileHistArrayY[12];
   TProfile* beamProfileCollArrayX[12];
   TProfile* beamProfileCollArrayY[12];
   TProfile* bProfileCollArrayX[12];
   TProfile* bProfileCollArrayY[12];

   char beamProfileHistName[100]; char Name[100]; char beamHistName[100];
   for (Int_t i=0; i<12; i++) {

     sprintf(beamProfileHistName,"beamProfileHistX%i",i);
     sprintf(Name,"|x| (mm)  for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TProfile* histX = new TProfile(beamProfileHistName,Name,nBins1,minBins1,maxBins1,"s");
     beamProfileHistArrayX[i]=histX;

     sprintf(beamHistName,"beamHistXt%i",i);
     sprintf(Name,"Beam envelope for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)1");}
     TH1F* hhhistX = new TH1F(beamHistName,Name,nBins1,minBins1,maxBins1);
     beamHistArrayX[i]=hhhistX;

     //

     sprintf(beamProfileHistName,"beamProfileHistY%i",i);
     sprintf(Name,"|y| (mm)  for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TProfile* histY = new TProfile(beamProfileHistName,Name,nBins1,minBins1,maxBins1,"s");
     beamProfileHistArrayY[i]=histY;

     sprintf(beamHistName,"beamHistY%i",i);
     sprintf(Name,"y of the beam envelope (mm)  for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TH1F* hhhistY = new TH1F(beamHistName,Name,nBins1,minBins1,maxBins1);
     beamHistArrayY[i]=hhhistY;

      //

     sprintf(beamProfileHistName,"bProfileHistX%i",i);
     sprintf(Name,"|x| (mm)  for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TProfile* histX = new TProfile(beamProfileHistName,Name,nBins1,minBins1,maxBins1,"s");
     bProfileHistArrayX[i]=histX;

     sprintf(beamHistName,"bHistXt%i",i);
     sprintf(Name,"Beam envelope for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)1");}
     TH1F* hhhistX = new TH1F(beamHistName,Name,nBins1,minBins1,maxBins1);
     bHistArrayX[i]=hhhistX;

     //

     sprintf(beamProfileHistName,"bProfileHistY%i",i);
     sprintf(Name,"|y| (mm)  for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TProfile* histY = new TProfile(beamProfileHistName,Name,nBins1,minBins1,maxBins1,"s");
     bProfileHistArrayY[i]=histY;

     sprintf(beamHistName,"bHistY%i",i);
     sprintf(Name,"y of the beam envelope (mm)  for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TH1F* hhhistY = new TH1F(beamHistName,Name,nBins1,minBins1,maxBins1);
     bHistArrayY[i]=hhhistY;

      //

     sprintf(beamProfileHistName,"beamProfileCollArrayX%i",i);
     sprintf(Name,"#sigma_{x at z=0} (mm) for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TProfile* histttX = new TProfile(beamProfileHistName,Name,nBins2,minBins2,maxBins2,"s");
     beamProfileCollArrayX[i]=histttX;

     sprintf(beamHistName,"beamCollArrayX%i",i);
     sprintf(Name,"Beam size at sample position for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TH1F* hhhistttX = new TH1F(beamHistName,Name,nBins2,minBins2,maxBins2);
     beamCollArrayX[i]=hhhistttX;
    
      //

     sprintf(beamProfileHistName,"beamProfileCollArrayY%i",i);
     sprintf(Name,"#sigma_{y at z=0} (mm) for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TProfile* histttY = new TProfile(beamProfileHistName,Name,nBins2,minBins2,maxBins2,"s");
     beamProfileCollArrayY[i]=histttY;

     sprintf(beamHistName,"beamCollArrayY%i",i);
     sprintf(Name,"y of the beam envelope (mm)  for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TH1F* hhhistttY = new TH1F(beamHistName,Name,nBins2,minBins2,maxBins2);
     beamCollArrayY[i]=hhhistttY;
      //

     sprintf(beamProfileHistName,"bProfileCollArrayX%i",i);
     sprintf(Name,"#sigma_{x at z=0} (mm) for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TProfile* histttX = new TProfile(beamProfileHistName,Name,nBins2,minBins2,maxBins2,"s");
     bProfileCollArrayX[i]=histttX;

     sprintf(beamHistName,"bCollArrayX%i",i);
     sprintf(Name,"x of the beam envelope (mm)  for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TH1F* hhhistttX = new TH1F(beamHistName,Name,nBins2,minBins2,maxBins2);
     bCollArrayX[i]=hhhistttX;
    
      //

     sprintf(beamProfileHistName,"bProfileCollArrayY%i",i);
     sprintf(Name,"#sigma_{y at z=0} (mm) for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TProfile* histttY = new TProfile(beamProfileHistName,Name,nBins2,minBins2,maxBins2,"s");
     bProfileCollArrayY[i]=histttY;

     sprintf(beamHistName,"bCollArrayY%i",i);
     sprintf(Name,"y of the beam envelope (mm)  for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TH1F* hhhistttY = new TH1F(beamHistName,Name,nBins2,minBins2,maxBins2);
     bCollArrayY[i]=hhhistttY;

     //
     sprintf(beamHistName,"beamTransmission%i",i);
     sprintf(Name,"Transmission in percent for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TH1F* htransm = new TH1F(beamHistName,Name,int(2*(maxBins2-minBins2)),minBins2,maxBins2);
     beamTransmission[i]=htransm;

     sprintf(beamHistName,"bTransmission%i",i);
     sprintf(Name,"Transmission in percent for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TH1F* htransm = new TH1F(beamHistName,Name,int(2*(maxBins2-minBins2)),minBins2,maxBins2);
     bTransmission[i]=htransm;
     
     sprintf(beamHistName,"beamSample%i",i);
     sprintf(Name,"Muons in sample in percent for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TH1F* htransm = new TH1F(beamHistName,Name,int(2*(maxBins2-minBins2)),minBins2,maxBins2);
     beamSample[i]=htransm;

     sprintf(beamHistName,"bSample%i",i);
     sprintf(Name,"Muons in sample in percent for  B=%iT; z (cm)",i);
     if (i==11) {sprintf(Name,"; z (cm)");}
     TH1F* htransm = new TH1F(beamHistName,Name,int(2*(maxBins2-minBins2)),minBins2,maxBins2);
     bSample[i]=htransm;

     sprintf(beamHistName,"beamSampleMap%i",i);
     sprintf(Name,"Fraction of #mu inside R_{sample}  B=%iT; R (mm)",i);
     if (i==11) {sprintf(Name,"; R (mm)");}
     TH1F* hbSM = new TH1F(beamHistName,Name,30,0.,30.);
     beamSampleMap[i]=hbSM;

     sprintf(beamHistName,"bSampleMap%i",i);
     sprintf(Name,"Fraction of #mu inside R_{sample}  B=%iT; R (mm)",i);
     if (i==11) {sprintf(Name,"; R (mm)");}
     TH1F* hbSM = new TH1F(beamHistName,Name,30,0.,30.);
     bSampleMap[i]=hbSM;

   }

   Int_t nEventsPerField[12];
   for (Int_t i=0; i<12; i++) {nEventsPerField[i]=0;}

   TH1F* hBeamDistr = new TH1F("hBeamDistr","hBeamDistr",100,0,50);
   TH1F* hR = new TH1F("hR",";R_{ini} (mm);N",32,0,80);
   TH1F* hRR = new TH1F("hRR",";R_{ini} (mm);N/R",32,0,80);
   TH1F* hX = new TH1F("hX",";x_{ini} (mm);N",32,-80,80);
   TH1F* hY = new TH1F("hY",";y_{ini} (mm);N",32,-80,80);
   TH1F* hP = new TH1F("hP",";p_{ini} (MeV/c);N",40,26,30);
 

   //   TProfile* hTest_prof = new TProfile("hTest_prof","Test of the profile",1,0,1.);
   //   Double_t rms=0;
   //   for (Int_t i=0; i<10; i++) {
   //     hTest_prof->Fill(0.5,float(i));
   //     rms+=(4.5-i)*(4.5-i);
   //   }
   //   rms=sqrt(rms)/10;
   //   cout<<"rms calculated="<<rms<<endl;
   //   TCanvas* cT= new TCanvas("cT","canvas T");
   //   hTest_prof->Draw();
   //   cT->Update();
   //   cout<<"hTest_prof: mean="<<hTest_prof->GetBinContent(1)<<"    error="<<hTest_prof->GetBinError(1)<<endl;

   //   TH1 *h;
   //   TKey *key;
   //   TIter nextkey(gDirectory->GetListOfKeys());
   //   while (key = (TKey*)nextkey()) {
   //     obj = key->ReadObj();
   //     if (obj->IsA()->InheritsFrom("TH1")) {
   //      h = (TH1*)obj; 
   //       std::cout << h->GetName() << std::endl;
   //       h->Sumw2();
   //     }
   //   }


   Long64_t nentries = fChain->GetEntriesFast();
   //   nentries=1000;

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if ((jentry%100)==0) {cout<<"analysing event nr. "<<jentry<<endl;}
      

      muDecayPosR=-1000;
      muTargetPolPhi=-1000;
      muTargetPolR = -1000;
      Double_t BFieldZ=-100;
      if (nFieldNomVal>0) {BFieldZ=fieldNomVal[0];}
      if (muDecayDetID==201) { 
	muDecayPosR=sqrt(muDecayPosX*muDecayPosX+muDecayPosY*muDecayPosY); 
	muTargetPolR=sqrt(muTargetPolX*muTargetPolX+muTargetPolY*muTargetPolY);
	muTargetPolPhi=atan2(muTargetPolY,muTargetPolX);
	if ((BFieldZ<3)||( (BFieldZ>5.5)&&(BFieldZ<7.5))) {
	  if (muTargetPolPhi<0) {muTargetPolPhi +=2*pi;}
	}
	else if ((BFieldZ>9  )&&(BFieldZ<10 )) {
	  if (muTargetPolPhi<-1.2) {muTargetPolPhi +=2*pi;}
	}
      }
      
      det_R_M0=-1000;
      for (Int_t i=0; i<det_n; i++) {
	if (det_ID[i]==102) {det_R_M0=sqrt(det_x[i]*det_x[i]+det_y[i]*det_y[i]);break;}
      }

      if ( (strcmp(fileNr,"100300")>0) ||
	   (muIniPosX<6) ) {
	hBeamR_T         = hBeamR_T_1;
	hBeamR_T_prof    = hBeamR_T_prof_1;
	hBeamR_M0        = hBeamR_M0_1;
	hBeamR_M0_prof   = hBeamR_M0_prof_1;
	hMuonPolPhi      = hMuonPolPhi_1;
	hMuonPolPhi_prof = hMuonPolPhi_prof_1;
	hMuonPolR        = hMuonPolR_1;
	hMuonPolR_prof   = hMuonPolR_prof_1;
      }
      else if (muIniPosX<11) {
	hBeamR_T         = hBeamR_T_2;
	hBeamR_T_prof    = hBeamR_T_prof_2;
	hBeamR_M0        = hBeamR_M0_2;
	hBeamR_M0_prof   = hBeamR_M0_prof_2;
	hMuonPolPhi      = hMuonPolPhi_2;
	hMuonPolPhi_prof = hMuonPolPhi_prof_2;
	hMuonPolR        = hMuonPolR_2;
	hMuonPolR_prof   = hMuonPolR_prof_2;
      }
      else if (muIniPosX<16) {
	hBeamR_T         = hBeamR_T_3;
	hBeamR_T_prof    = hBeamR_T_prof_3;
	hBeamR_M0        = hBeamR_M0_3;
	hBeamR_M0_prof   = hBeamR_M0_prof_3;
	hMuonPolPhi      = hMuonPolPhi_3;
	hMuonPolPhi_prof = hMuonPolPhi_prof_3;
	hMuonPolR        = hMuonPolR_3;
	hMuonPolR_prof   = hMuonPolR_prof_3;
      }
      else {
	hBeamR_T         = hBeamR_T_4;
	hBeamR_T_prof    = hBeamR_T_prof_4;
	hBeamR_M0        = hBeamR_M0_4;
	hBeamR_M0_prof   = hBeamR_M0_prof_4;
	hMuonPolPhi      = hMuonPolPhi_4;
	hMuonPolPhi_prof = hMuonPolPhi_prof_4;
	hMuonPolR        = hMuonPolR_4;
	hMuonPolR_prof   = hMuonPolR_prof_4;
      }


// Filling
      if (muDecayPosR>=0) {
	hBeamR_T        -> Fill(BFieldZ,muDecayPosR);
	hBeamR_T_prof   -> Fill(BFieldZ,muDecayPosR);
	hMuonPolPhi     -> Fill(BFieldZ,muTargetPolPhi);
	hMuonPolPhi_prof-> Fill(BFieldZ,muTargetPolPhi);
	hMuonPolR       -> Fill(BFieldZ,muTargetPolR);
	hMuonPolR_prof  -> Fill(BFieldZ,muTargetPolR);
      }
      if (det_R_M0>=0) {
	hBeamR_M0      -> Fill(BFieldZ,det_R_M0);
	hBeamR_M0_prof -> Fill(BFieldZ,det_R_M0);
      }

      Bool_t M0Hit=kFALSE;
      Bool_t SampleHit=kFALSE;
      for (Int_t i=0; i<det_n; i++) {
	if (det_ID[i]==102) {M0Hit=kTRUE;}
	//	else if (det_ID[i]==201) {SampleHit=kTRUE;}
      }
      if (muDecayDetID==201) {SampleHit=kTRUE;}

      hEff0 -> Fill(BFieldZ,1);
      if (M0Hit&&SampleHit)    {hEff1 -> Fill(BFieldZ,1);}
      if (M0Hit&&(!SampleHit)) {hEff2 -> Fill(BFieldZ,1);}
      if ((!M0Hit)&&SampleHit) {hEff3 -> Fill(BFieldZ,1);}     

      Int_t intBFieldZ=int(BFieldZ+0.5);
      if (fabs(BFieldZ-intBFieldZ)<0.05) {
	//	TProfile* hist = beamProfileHistArray[intBFieldZ];
	TProfile* histX = beamProfileHistArrayX[intBFieldZ];
	TProfile* histY = beamProfileHistArrayY[intBFieldZ];
	TProfile* histX2 = bProfileHistArrayX[intBFieldZ];
	TProfile* histY2 = bProfileHistArrayY[intBFieldZ];
	TProfile* hCollX = beamProfileCollArrayX[intBFieldZ];
	TProfile* hCollY = beamProfileCollArrayY[intBFieldZ];
	TProfile* hColl2X = bProfileCollArrayX[intBFieldZ];
	TProfile* hColl2Y = bProfileCollArrayY[intBFieldZ];
	TH1F* htransm = beamTransmission[intBFieldZ];
	TH1F* htransm2 = bTransmission[intBFieldZ];
	TH1F* hsample = beamSample[intBFieldZ];
	TH1F* hsample2 = bSample[intBFieldZ];
	TH1F* hbSM = beamSampleMap[intBFieldZ];
	TH1F* hbSM2 = bSampleMap[intBFieldZ];
	

	Bool_t ignore_rest = false;
	// Find the position of the muon in the centre 
	Double_t saveR0=-99999;	Double_t savex0=-99999; Double_t savey0=-99999; Double_t save_R2=-99999;
	Double_t saveRbeg==-99999;
	for (Int_t i=0; i<save_n; i++) {
	  if ((save_detID[i]==save_detID_firstplain)&&(save_particleID[i]==-13)) {nEventsPerField[intBFieldZ]++;}
	  if (save_detID[i]==save_detID_sample) {
	    if (save_particleID[i]==-13) {
	      saveR0=sqrt(save_x[i]*save_x[i]+save_y[i]*save_y[i]);
	      savex0=save_x[i];// cout<<savex0<<"\t";
	      savey0=save_y[i];// cout<<savey0<<endl;
	    }
	  }
	  if (save_detID[i]==save_detID_coll2) {
	    if (save_particleID[i]==-13) {
	      save_R2=sqrt(save_x[i]*save_x[i]+save_y[i]*save_y[i]);
	    }
	  }
	  if (save_detID[i]==save_detID_firstplain) {
	    if (save_particleID[i]==-13) {
	      saveRbeg=sqrt(save_x[i]*save_x[i]+save_y[i]*save_y[i]);
	    }
	  }

	}
	//	if (saveR0>40.) continue;

	for (Int_t i=0; i<save_n; i++) {
	  if (save_particleID[i]==-13) {
	    //	    cout<<"save_detID="<<save_det
	    Double_t save_R=sqrt(save_x[i]*save_x[i]+save_y[i]*save_y[i]);
	    //	    Double_t rounded_z=(int(save_z[i]))/10.;
	    //	    Int_t idOfSaveVolume=zPosToSaveID_map[rounded_z];
	    //	    if (idOfSaveVolume!=0) {
	    if (save_R<R_collim1) {
	      if (savex0!=-99999) { 
		hCollX->Fill(save_z[i]/10.,savex0); 
		htransm->Fill(save_z[i]/10.); 
		if (saveR0<sampleRadius) {hsample->Fill(save_z[i]/10.);}
	      }
	      if (savey0!=-99999) { hCollY->Fill(save_z[i]/10.,savey0);}
	      if (save_R2<R_collim2) {
		if (savex0!=-99999) { 
		  hColl2X->Fill(save_z[i]/10.,savex0); 
		  htransm2->Fill(save_z[i]/10.);
		  if (saveR0<sampleRadius) {hsample2->Fill(save_z[i]/10.);}
		}
		if (savey0!=-99999) { hColl2Y->Fill(save_z[i]/10.,savey0);}
	      }
	    }
	    //if ((save_detID[i]==501)&&(save_R>10.)) { ignore_rest = true;}
	    if (!ignore_rest) {
	      //	      hist->Fill(save_z[i]/10.,save_R);
	      histX->Fill(save_z[i]/10.,save_x[i]);
	      histY->Fill(save_z[i]/10.,save_y[i]);
	      //	      if ((saveRbeg>=0.)&&(saveRbeg<40.)) {
	      if (save_R2<R_collim2) {
		histX2->Fill(save_z[i]/10.,save_x[i]);
		histY2->Fill(save_z[i]/10.,save_y[i]);
	      }
	    }
	    if ((intBFieldZ==0)&&(save_detID[i]==save_detID_firstplain)) { hBeamDistr->Fill(save_R); }
	  }
	}
	hbSM->Fill(saveR0);
	if (save_R2<R_collim2) hbSM2->Fill(saveR0);
	
      }
      Double_t muIniPosR=sqrt(muIniPosX*muIniPosX+muIniPosY*muIniPosY);
      hR->Fill(muIniPosR);
      if (muIniPosR!=0) {hRR->Fill(muIniPosR,1./muIniPosR);}
      hX->Fill(muIniPosX);
      hY->Fill(muIniPosY);
      hP->Fill(sqrt(muIniMomX*muIniMomX+muIniMomY*muIniMomY+muIniMomZ*muIniMomZ));


   }
   //   TProfile* hist = beamProfileHistArrayX[0];
   //   for (int i=0; i<10; i++) {
   //     cout<<"histX="<<hist<<"  mean at -100cm at 0T ="<<hist->GetBinContent(i)<<"  RMS="<<hist->GetBinError(i)<<endl;
   //   }
   


   c1->Divide(2,2);
   c1->cd(1);
   hBeamR_T_1->Draw();
   hBeamR_T_2->Draw("same");
   hBeamR_T_3->Draw("same");
   hBeamR_T_4->Draw("same");
   TPaveLabel* pel = new TPaveLabel(0.12,0.82,0.77,0.92,cLabel,"brNDC");
   TPaveLabel* pel2 = new TPaveLabel(0.05,0.57,0.95,0.75,cLabel2,"brNDC");
   pel2->SetTextSize(0.3);
   pel->Draw();
   c1->cd(2);
   gPad->SetGrid();
   hBeamR_T_prof_1->SetMaximum(25);
   hBeamR_T_prof_1->Draw();
   hBeamR_T_prof_2->Draw("same");
   hBeamR_T_prof_3->Draw("same");
   hBeamR_T_prof_4->Draw("same");
   c1->cd(3);
   hBeamR_M0_1->Draw();
   hBeamR_M0_2->Draw("same");
   hBeamR_M0_3->Draw("same");
   hBeamR_M0_4->Draw("same");
   c1->cd(4);
   gPad->SetGrid();
   hBeamR_M0_prof_1->SetMaximum(25);
   hBeamR_M0_prof_1->Draw();
   hBeamR_M0_prof_2->Draw("same");
   hBeamR_M0_prof_3->Draw("same");
   hBeamR_M0_prof_4->Draw("same");
   sprintf(filenamePrint,"Beam_%s.ps",fileNr);
   c1->Print(filenamePrint);

   c2->Divide(2,2);
   c2->cd(1);
   hMuonPolPhi_1->Draw();
   hMuonPolPhi_2->Draw("same");
   hMuonPolPhi_3->Draw("same");
   hMuonPolPhi_4->Draw("same");
   pel->Draw();
   c2->cd(2);
   gPad->SetGrid();
   hMuonPolPhi_prof_1->Draw();
   hMuonPolPhi_prof_2->Draw("same");
   hMuonPolPhi_prof_3->Draw("same");
   hMuonPolPhi_prof_4->Draw("same");
   c2->cd(3);
   hMuonPolR_1->Draw();
   hMuonPolR_2->Draw("same");
   hMuonPolR_3->Draw("same");
   hMuonPolR_4->Draw("same");
   c2->cd(4);
   gPad->SetGrid();
   hMuonPolR_prof_1->SetMaximum(1.01);
   hMuonPolR_prof_1->SetMinimum(0.92);
   hMuonPolR_prof_1->Draw();
   hMuonPolR_prof_2->Draw("same");
   hMuonPolR_prof_3->Draw("same");
   hMuonPolR_prof_4->Draw("same");
   sprintf(filenamePrint,"Beam_pol_%s.ps",fileNr);
   c2->Print(filenamePrint);

   //   c3->Divide(2,2);
   c3->cd(2);
   hEff1_Norm->Divide(hEff1,hEff0,1.,1.);
   hEff1_Norm->Draw();
   c3->cd(1);
   pel->Draw("pe");
   c3->cd(3);
   hEff2_Norm->Divide(hEff2,hEff0,1.,1.);
   hEff2_Norm->Draw("pe");
   c3->cd(4);
   hEff3_Norm->Divide(hEff3,hEff0,1.,1.);
   hEff3_Norm->Draw("pe");
   sprintf(filenamePrint,"Beam_eff_%s.ps",fileNr);
   c3->Print(filenamePrint);

   gStyle->SetOptStat(0);
     //   Double_t maxYbeam=234;  Double_t minYbeam=0;
     //   Double_t maxYYYbeam=5;  Double_t minYYYbeam=0;
   Double_t maxYbeam=100;  Double_t minYbeam=-100;
   Double_t maxYYYbeam=20;  Double_t minYYYbeam=-20;
   Double_t maxYcol=30;  Double_t minYcol=-30;
   //   for (Int_t i=0; i<12; i++) {
   //     c4->cd(i+1);
   //     TProfile* hist=beamProfileHistArray[i];
   //     hist->SetMaximum(maxYbeam);
   //     hist->SetMinimum(minYbeam);
   //     hist->DrawCopy();
     //     c8->cd(i+1);
     //     hist->SetMaximum(maxYYYbeam);
     //     hist->SetMinimum(minYYYbeam);
     //     hist->GetXaxis()->SetRange(350,550);
     //     hist->DrawCopy();
   //   }
   //   c4->cd(12);
   //   pel->Draw();
   //   sprintf(filenamePrint,"Beam_profile%s_%s.ps",colimExtention,fileNr);
   //   c4->Print(filenamePrint);
   //   c8->cd(12);
   //   pel->Draw();
   //   sprintf(filenamePrint,"Beam_profile_detail%s_%s.ps",colimExtention,fileNr);
   //   c8->Print(filenamePrint);

   //   TLegend *legend = new TLegend(0.80,0.75,0.98,0.98);
   //   TLegend *legend2 = new TLegend(0.70,0.70,0.98,0.98);

   TLegend *legend = new TLegend(0.20,0.12,0.6,0.55);
   TLegend *legend2 = new TLegend(0.20,0.12,0.6,0.55);
   TLegend *legend3 = new TLegend(0.20,0.12,0.6,0.55);

   for (Int_t i=0; i<12; i++) {
     c5->cd(i+1);
     TProfile* histX=beamProfileHistArrayX[i];
     TH1F* hXXX = beamHistArrayX[i];
     ConvertProfileTo1dHist(histX, hXXX, "x");
     SetMinMaxColAndDrawCopy(hXXX,minYbeam,maxYbeam,red,20,0.3,"PC");
     TProfile* histX2=bProfileHistArrayX[i];
     TH1F* hXXX2 = bHistArrayX[i];
     ConvertProfileTo1dHist(histX2, hXXX2, "x");
     SetMinMaxColAndDrawCopy(hXXX2,minYbeam,maxYbeam,magenta,20,0.3,"PCsame");

     TProfile* histY=beamProfileHistArrayY[i];
     TH1F* hYYY = beamHistArrayY[i];
     ConvertProfileTo1dHist(histY, hYYY, "y");
     SetMinMaxColAndDrawCopy(hYYY,minYbeam,maxYbeam,green,20,0.3,"PCsame");
     TProfile* histY2=bProfileHistArrayY[i];
     TH1F* hYYY2 = bHistArrayY[i];
     ConvertProfileTo1dHist(histY2, hYYY2, "y");
     SetMinMaxColAndDrawCopy(hYYY2,minYbeam,maxYbeam,lightblue,20,0.3,"PCsame");

     if (i==0) {
       legend->AddEntry(hXXX,"2 #sigma_{x}","l");
       legend->AddEntry(hYYY,"-2 #sigma_{y}","l");
       legend->AddEntry(hXXX2,"2 #sigma_{x} (subset)","l");
       legend->AddEntry(hYYY2,"-2 #sigma_{y} (subset)","l");
       legend->SetTextSize(0.08);
       //       legend->Draw();
       //       gPad->Update();
     }

     c6->cd(i+1);
     //     hXXX->GetXaxis()->SetRange(70,130);
     hXXX->GetXaxis()->SetRangeUser(-30.,30.);
     SetMinMaxColAndDrawCopy(hXXX,minYYYbeam,maxYYYbeam,red,20,0.3,"PC");
     hXXX2->GetXaxis()->SetRangeUser(-30.,30.);
     SetMinMaxColAndDrawCopy(hXXX2,minYYYbeam,maxYYYbeam,magenta,20,0.3,"PCsame");
     //     hYYY->GetXaxis()->SetRange(70,130);
     hYYY->GetXaxis()->SetRangeUser(-30.,30.);
     SetMinMaxColAndDrawCopy(hYYY,minYYYbeam,maxYYYbeam,green,20,0.3,"PCsame");
     hYYY2->GetXaxis()->SetRangeUser(-30.,30.);
     SetMinMaxColAndDrawCopy(hYYY2,minYYYbeam,maxYYYbeam,lightblue,20,0.3,"PCsame");


     c8->cd(i+1);
     TProfile* histcoX=beamProfileCollArrayX[i];
     TH1F* hcoXXX = beamCollArrayX[i];
     ConvertProfileTo1dHist(histcoX, hcoXXX, "x");
     SetMinMaxColAndDrawCopy(hcoXXX,minYcol,maxYcol,red,20,0.3,"PC");
     TProfile* histcoX2=bProfileCollArrayX[i];
     TH1F* hcoXXX2 = bCollArrayX[i];
     ConvertProfileTo1dHist(histcoX2, hcoXXX2, "x");
     SetMinMaxColAndDrawCopy(hcoXXX2,minYcol,maxYcol,magenta,20,0.3,"PCsame");

     TProfile* histcoY=beamProfileCollArrayY[i];
     TH1F* hcoYYY = beamCollArrayY[i];
     ConvertProfileTo1dHist(histcoY, hcoYYY, "y");
     SetMinMaxColAndDrawCopy(hcoYYY,minYcol,maxYcol,green,20,0.3,"PCsame");
     TProfile* histcoY2=bProfileCollArrayY[i];
     TH1F* hcoYYY2 = bCollArrayY[i];
     ConvertProfileTo1dHist(histcoY2, hcoYYY2, "y");
     SetMinMaxColAndDrawCopy(hcoYYY2,minYcol,maxYcol,lightblue,20,0.3,"PCsame");

     if (i==0) {
       legend2->AddEntry(hcoXXX,"2 #sigma_{x at z=0}","l");
       legend2->AddEntry(hcoXXX2,"2 #sigma_{x at z=0} 2coll.","l");
       legend2->AddEntry(hcoYYY,"-2 #sigma_{y zt z=0}","l");
       legend2->AddEntry(hcoYYY2,"-2 #sigma_{y zt z=0} 2coll","l");
       legend2->SetTextSize(0.08);
     }

     c9->cd(i+1);
     TH1F* hbSM = beamSampleMap[i];
     TH1F* hbSM2 = bSampleMap[i];
     SetMinMaxColAndDrawCopy(hbSM,0.,0.,red,20,0.3,"hist");
     SetMinMaxColAndDrawCopy(hbSM2,0.,0.,blue,20,0.3,"hist same");




     c10->cd(i+1);
     TH1F* htransm =  beamTransmission[i];
     TH1F* htransm2 =  bTransmission[i];
     TH1F* hsample = beamSample[i];
     TH1F* hsample2 = bSample[i];
     if (nEventsPerField[i]!=0) {
       htransm->Scale(100./nEventsPerField[i]);
       htransm2->Scale(100./nEventsPerField[i]);
       hsample->Scale(100./nEventsPerField[i]);
       hsample2->Scale(100./nEventsPerField[i]);
     }
     gPad->SetLogy();
     SetMinMaxColAndDrawCopy(htransm,0.1,150.,red,20,0.3,"PC");
     SetMinMaxColAndDrawCopy(htransm2,0.1,150.,blue,20,0.3,"PCsame");
     hsample->SetLineStyle(2);  hsample->SetLineWidth(2);
     hsample2->SetLineStyle(2); hsample2->SetLineWidth(2);
     SetMinMaxColAndDrawCopy(hsample,0.1,150.,red,20,0.3,"hist same");
     SetMinMaxColAndDrawCopy(hsample2,0.1,150.,blue,20,0.3,"hist same");
     if (i==0) {
       legend3->AddEntry(htransm,"1 collimator","l");
       legend3->AddEntry(htransm2,"2 collimators","l");
       legend3->AddEntry(hsample,"in sample (1 coll.)","l");
       legend3->AddEntry(hsample2,"in sample (2 coll.)","l");
     }
   }
   c5->cd(12);
   legend->Draw();
   pel->Draw();
   pel2->Draw();
   gPad->Update();
   sprintf(filenamePrint,"Beam_envelope%s_%s.ps",colimExtention,fileNr);
   c5->Print(filenamePrint);
   c6->cd(12);
   legend->Draw();
   pel->Draw();
   pel2->Draw();
   gPad->Update();
   sprintf(filenamePrint,"Beam_envelope_detail%s_%s.ps",colimExtention,fileNr);
   c6->Print(filenamePrint);

   //      gStyle->SetOptStat(1110);
   c7->cd(1);
   //   hBeamDistr->Draw();
   hP->Draw();
   hP->SetLineWidth(2);
   hP->Fit("gaus");
   hP->GetFunction("gaus")->SetLineWidth(1.5);
   c7->cd(2);
   hY->Draw();
   hY->SetLineWidth(2);
   hY->Fit("gaus");
   hY->GetFunction("gaus")->SetLineWidth(1.5);
   c7->cd(3);
   hX->Draw();
   hX->SetLineWidth(2);
   hX->Fit("gaus");
   hX->GetFunction("gaus")->SetLineWidth(1.5);
   c7->cd(4);
   hR->Draw();
   hR->SetLineWidth(2);
   sprintf(filenamePrint,"Beam_c7%s_%s.ps",colimExtention,fileNr);
   c7->Print(filenamePrint);

   c8->cd(12);
   pel->Draw();
   pel2->Draw();
   legend2->Draw();
   gPad->Update();
   sprintf(filenamePrint,"Beam_c8%s_%s.ps",colimExtention,fileNr);
   c8->Print(filenamePrint);


   c9->cd(12);
   pel->Draw();
   pel2->Draw();
   //   legend2->Draw();
   gPad->Update();
   sprintf(filenamePrint,"Beam_c9%s_%s.ps",colimExtention,fileNr);
   c9->Print(filenamePrint);


   //   c9->cd(1);
   //   beamCollArrayX[0]->Draw();
   //   c9->cd(2);
   //   beamCollArrayY[0]->Draw();
   //   c9->cd(3);
   //   beamProfileCollArrayX[0]->Draw();
   //   c9->cd(4);
   //   beamProfileCollArrayY[0]->Draw();

   c10->cd(12);
   legend3->Draw();
   pel->Draw();
   pel2->Draw();
   gPad->Update();
   sprintf(filenamePrint,"Beam_c10%s_%s.ps",colimExtention,fileNr);
   c10->Print(filenamePrint);
  

}


void ConvertProfileTo1dHist(TProfile* hProf, TH1F* h, char* coordinate) {
  Int_t nbins = hProf->GetNbinsX();
  Double_t znaminko=1.;
  if (strcmp(coordinate,"y")==0) {znaminko=-1.;}
  for (Int_t i = 0; i<nbins; i++) {
    Double_t sigma = hProf->GetBinError(i);
    //    cout<<"\t sigma="<<sigma;
    h->SetBinContent(i,2*sigma*znaminko);
  }
  //  cout<<endl;
}


void SetMinMaxColAndDrawCopy(TH1F* hist, Double_t min, Double_t max, Int_t colour, Int_t style, Double_t size, char* option) {
  gPad->SetGrid();
  if (max>min) {
    hist->SetMaximum(max);
    hist->SetMinimum(min);
  }
  hist->SetMarkerStyle(style);
  hist->SetMarkerSize(size);
  hist->SetMarkerColor(colour);
  hist->SetLineColor(colour);
  hist->DrawCopy(option);
}


//void Beam::FillIndCol1(Int_t nMax, Int_t* index) {
//  for
//
//}
