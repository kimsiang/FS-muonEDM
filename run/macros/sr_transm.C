// To run root without logon screen and script use: root -l -n
// To run a named macro in root use:
//.L sr_transm.C
// sr_transm("data/sr1_1100.root")

#include <string>
void sr_transm(char* fname)
{

TFile* f1 = new TFile(fname);

//TCanvas* c1 = new TCanvas("c1","Polarization and Transmission",60,40,800,800);
gStyle->SetPalette(1,0);
TCanvas* c1 = new TCanvas("c1","Polarization and Transmission",60,40,800,800);
c1->Divide(2,2);
c1->SetGridx();
c1->SetGridy();
//c1->cd(1)->SetGridx();c1->cd(1)->SetGridy();

c1->cd(4)->SetGridx();c1->cd(4)->SetGridy(); // c1->cd(3);
t1->Draw("muTargetPosY[0]:muTargetPosX[0]>>bspot_hist(64,-40,40,64,-40,40)","muTargetPolX > -2");
//t1->Draw("det_y:det_x>>bspot_hist(64,-80,80,64,-80,80)"); //t1->Draw("mcpHit.positiony*10:mcpHit.positionx*10>>bspot_hist(64,-40,40,64,-40,40)","mcpHit.positionz==2&&mcpHit.ID==0");
bspot_hist->SetTitle("Muon beam cross section [mm]");
//bspot_hist->Draw("cont0");
bspot_hist->Draw();
bspot_hist->Draw("cont0 same");
// t1->Draw("mcpHit.positionx*10:mcpHit.positiony*10>>bspot_hist(128,-12,12,128,-12,12)","","goff");
bspot_hist->ProjectionX("xsect",31,33);
bspot_hist->ProjectionY("ysect",31,33);

// TText *pt = new TText(-36., 33., "L1 = 11.0 kV");

// pt->SetTextSize(0.05);
// pt->Draw();


//pl=new TPaveLabel(2, 155, 16, 175, "RA_Up = 11 kV");
//pl=new TPaveLabel(-75, 60, -15, 75, "#splitline{RA_Up = 8.6 kV}{L3 = 6.78 kV}");
//pl->SetBorderSize(1);
//pl->SetTextSize(0.43);
//pl->Draw();



c1->cd(3);
xsect->SetTitle("x cross section [mm]");
xsect->SetFillStyle(1001);
//xsect->Draw("F");
xsect->SetFillColor(45);
xsect->Draw();

c1->cd(2);
t1->Draw("muTargetPolZ >> pol_z(128, -1, -0.95)","(muTargetPolZ > -2)"); //-1., 1
pol_z->SetTitle("Polarization Z");
pol_z->GetXaxis()->SetNdivisions(405);
pol_z->GetYaxis()->SetNdivisions(406);
pol_z->GetXaxis()->SetTickLength(0.018);
pol_z->GetYaxis()->SetTickLength(0.018);
pol_z->Draw("F");
pol_z->SetFillStyle(1001);
pol_z->SetFillColor(38);
pol_z->Draw();


c1->cd(1);
//Energy loss in CFoil
//t1->Draw("10-det_edep_mup*1000","(det_n==1)&&(det_edep<0.1)&&(det_edep_mup*1000>0.1)");
t1->Draw("muTargetPolX >> pol_x(128, -0.4, 0)","(muTargetPolX > -2)"); //0, 2
//t1->Draw("det_edep_mup*1000","(det_n==1)&&(det_edep<0.1)");
//t1->Draw("det_edep_mup*1000>>tmp_hist(64,-80,80)","(det_n==1)&&(det_edep<0.1)");
pol_x->SetTitle("Polarization X");
pol_x->GetXaxis()->SetNdivisions(405);
pol_x->GetYaxis()->SetNdivisions(406);
pol_x->GetXaxis()->SetTickLength(0.018);
pol_x->GetYaxis()->SetTickLength(0.018);
pol_x->Draw("F");
pol_x->SetFillStyle(1001);
//pol_x->SetFillColor(kBlue-5);
pol_x->SetFillColor(kGreen-5);
pol_x->Draw();
/*
TAxis* yax = ang_hist->GetYaxis();
yax->SetNdivisions(206);  // N2*100 + N1 (N2 = secondary, N1 = primary) //yax->SetNdivisions(410);
yax->SetTickLength(0.018);
yax->SetLabelSize(0.04);
yax->SetTitleSize(0.045);
yax->SetTitleOffset(1.3);
yax->SetTitle("Counts (arb. units)");
*/

c1->SaveAs("data/sr_bspot_0.eps");

}

/*

/// Plot of muon beam angular dispersion after lens 1:

TCanvas* c2 = new TCanvas("c2","Angular Dispersion",60,40,800,800);
//gStyle->SetPalette(1,0);
//c1->Divide(2,2);
c2->SetGridx();
c2->SetGridy();
//c2->SetFillColor(kWhite); // Fill background with white for printing

//c2->cd(1);
t1->Draw("save_pz","(save_detID==800)"); //xsect->SetTitle("x cross section [mm]");
htemp->SetTitle("Position 1 [mm]");
htemp->Draw("F");
htemp->SetFillStyle(1001);
htemp->SetFillColor(38);
htemp->Draw();

//t1->Draw("atan2(mcpHit.posyini,mcpHit.posxini)*57.29577951308:atan2(mcpHit.positiony,mcpHit.positionx)*57.29577951308>

// Double_t Pi = TMath::Pi(); // 180/Pi
//c2->cd(2)->SetGridx(); c1->cd(2)->SetGridy();
//t1->Draw("atan2(save_py,save_px)*57.29577951308:atan2(mcpHit.positiony,mcpHit.positionx)*57.29577951308>>bspot_hist3(64,-180,180,64,-180,180)","mcpHit.ID==0");
///t1->Draw("atan2(save_py,save_px)*57.29577951308");

//t1->Draw("det_y[0]:det_x[0]>>bspot_hist(64,-40,40,64,-40,40)");
t1->Draw("90-atan2(save_pz,sqrt(save_px^2 + save_py^2))*57.29577951308>>ang_hist(64,0,5)","(save_detID==900)");
//t1->Draw("90-atan2(save_pz,sqrt(save_px^2 + save_py^2))*57.29577951308");
//htemp->SetTitle("Correlation angle for L1 (8.7 kV, 15 keV); Final angle [deg.]; Inital angle [deg.]");
ang_hist->GetXaxis()->SetRangeUser(-5., 5.);
ang_hist->Draw("F");
ang_hist->SetFillStyle(1001);
ang_hist->SetFillColor(805);

ang_hist->SetTitleSize(0.025);
ang_hist->SetTitle("Muon angular distribution after L1");

ang_hist->GetXaxis()->SetNdivisions(206);
ang_hist->GetXaxis()->SetTickLength(0.018);
ang_hist->GetXaxis()->SetLabelSize(0.04);
ang_hist->GetXaxis()->SetTitleSize(0.045);
ang_hist->GetYaxis()->SetTitleOffset(1.0);
ang_hist->GetXaxis()->SetTitle("Angle (deg.)");

TAxis* yax = ang_hist->GetYaxis();
yax->SetNdivisions(206);  // N2*100 + N1 (N2 = secondary, N1 = primary) //yax->SetNdivisions(410);
yax->SetTickLength(0.018);
yax->SetLabelSize(0.04);
yax->SetTitleSize(0.045);
yax->SetTitleOffset(1.3);
yax->SetTitle("Counts (arb. units)");

ang_hist->Draw();

TText *pt = new TText(3.2, 200., "Plane at -300 mm");
pt->SetTextSize(0.03);
pt->Draw();

//TPaveText *pt = new TPaveText(0.6,0.4,0.89, 0.45,"trNDC"); 
//pt->SetTextSize(0.03);
//pt->SetFillColor(0); //pt->SetFillColor(390);
//pt->SetBorderSize(0);
//pt->SetTextAlign(12);
//pte = pt->AddText("Plane at z = -300 mm");
//pt->Draw();


//c2->SaveAs("data/sr1_ang_dist_300.eps");

}
*/

/// Insert simple text directly from command line
//  TText text(3, 300, "Plane at -380 mm");
//  text.SetTextSize(0.03);
//  text.Draw();


/// Optional changes of statistics
//  TPaveStats* sb2=(TPaveStats *)(ang_hist->GetListOfFunctions()->FindObject("stats"));
//  sb2->SetTextSize(0.025);
//  sb2->SetFillColor(390);


/// An easy way to plot functions
// (new TF1("fun1","(1-x)*sqrt(1-x*x)",-1,1))->Draw();
// (new TF2("fun2","(175/80)**2*(1-y)*(1-x**2)+(1+y)*(1-x)**2",-1,1,-1,1))->Draw("lego")


//  From: www-pnp.physics.ox.ac.uk/~west/root/plotting_with_style.html#demo_background
/// For publishing (insert these lines in rootlogon.C):
//  gStyle->SetLineWidth(2.);
//  gStyle->SetTextSize(1.1);
//  gStyle->SetLabelSize(0.06,"xy");
//  gStyle->SetTitleSize(0.06,"xy");
//  gStyle->SetTitleOffset(1.2,"x");
//  gStyle->SetTitleOffset(1.0,"y");
//  gStyle->SetPadTopMargin(0.1);
//  gStyle->SetPadRightMargin(0.1);
//  gStyle->SetPadBottomMargin(0.16);
//  gStyle->SetPadLeftMargin(0.12);

