// To run root without logon screen and script use: root -l -n
// To run a named macro in root use:
//.L td_mcp2_tof.C
// td_mcp2_tof("data/sr1_1100.root")

#include <string>
#define NSIZE 100000
void td_mcp2_tof(const char* fname)
{
  Double_t *ptofTD, *ptofMCP2, *pEventIDTD, *pEventIDMCP2;
  Double_t tofTD[NSIZE], tofMCP2[NSIZE], EventIDTD[NSIZE], EventIDMCP2[NSIZE];
  Long64_t i, j, k, nEventsTD, nEventsMCP2;
  TTree *t1;
  TH1D *htof, *tof_mcp2, *tof_td;
  
  TFile* f1 = new TFile(fname);
  t1 = (TTree*) gDirectory->FindObjectAny("t1");

  // muon time-of-flight to TD
  t1->Draw("eventID:1000*save_time","save_detID==890&&save_particleID==-13"); //0, 2
  nEventsTD  = t1->GetSelectedRows();
  ptofTD     = t1->GetV2();
  pEventIDTD = t1->GetV1();

  for (i = 0; i<nEventsTD; i++){
   EventIDTD[i] = pEventIDTD[i];
   tofTD[i]     = ptofTD[i];
  }

  // muon time-of-flight to MCP2, mu+
  t1->Draw("eventID:1000*save_time","save_detID==902&&save_particleID==-13"); //0, 2
  nEventsMCP2  = t1->GetSelectedRows();
  ptofMCP2     = t1->GetV2();
  pEventIDMCP2 = t1->GetV1();

  for (i = 0; i<nEventsMCP2; i++){
   EventIDMCP2[i] = pEventIDMCP2[i];
   tofMCP2[i]     = ptofMCP2[i];
  }

  // TOF TD-MCP2
  htof = new TH1D("Time-of-flight TD-MCP2", "Time-of-flight TD-MCP2", 1000, 0.25, 500.25);
  k = 0;
  for (i = 0; i<nEventsMCP2; i++){
   for (j = k; j<nEventsTD; j++){
   //  cout << i << ", " << j << ", " << EventIDMCP2[i] << ", " << EventIDTD[j] << ", " << tofMCP2[i] << ", " << tofTD[j] << endl;
     if (EventIDMCP2[i] == EventIDTD[j]){
       htof->Fill(tofMCP2[i] - tofTD[j]);
       k = j + 1;
       break;
     }
   }
  }

  gStyle->SetPalette(1,0);
//  gStyle->SetName(fname);
  gStyle->SetOptTitle(1);
  TCanvas* c1 = new TCanvas("Time of Flight","Time of Flight",60,40,600,800);
  c1->Divide(1,3);
  c1->SetGridx();
  c1->SetGridy();

  // muon time-of-flight to TD
  c1->cd(1);
  t1->Draw("1000*save_time >> tof_td(2000, 0.25, 1000.25)","save_detID==890&&save_particleID==-13"); //0, 2
  tof_td = (TH1D*) gDirectory->Get("tof_td");
  tof_td->SetTitle("Time-of-flight to TD");
  tof_td->GetXaxis()->SetTitle("TD time of flight (ns)");
//  tof_td->GetXaxis()->SetNdivisions(405);
  tof_td->GetYaxis()->SetNdivisions(406);
//  tof_td->GetXaxis()->SetTickLength(0.018);
  tof_td->GetYaxis()->SetTickLength(0.018);
  tof_td->Draw("F");
  tof_td->SetFillStyle(1001);
  tof_td->SetFillColor(kGreen-5);
  tof_td->Draw();
  //c1->SaveAs("data/sr_bspot_0.eps");

  // muon time-of-flight to MCP2
  c1->cd(2);
  t1->Draw("1000*save_time >> tof_mcp2(2000, 0.25, 1000.25)","save_detID==902&&((save_particleID==-13)||(save_particleID==-1313))"); //0, 2
  tof_mcp2 = (TH1D*) gDirectory->Get("tof_mcp2");
  tof_mcp2->SetTitle("Time-of-flight to MCP2");
  tof_mcp2->GetXaxis()->SetTitle("MCP2 time of flight (ns)");
//  tof_mcp2->GetXaxis()->SetNdivisions(405);
  tof_mcp2->GetYaxis()->SetNdivisions(406);
//  tof_mcp2->GetXaxis()->SetTickLength(0.018);
  tof_mcp2->GetYaxis()->SetTickLength(0.018);
  tof_mcp2->Draw("F");
  tof_mcp2->SetFillStyle(1001);
  tof_mcp2->Draw();

  c1->cd(3);
  htof->SetTitle("Time-of-flight TD-MCP2");
  htof->GetXaxis()->SetTitle("TD-MCP2 time of flight (ns)");
  htof->GetYaxis()->SetNdivisions(406);
  htof->GetYaxis()->SetTickLength(0.018);
  htof->Draw("F");
  htof->Draw();
}


