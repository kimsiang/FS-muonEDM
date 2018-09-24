{
 ////////////////////////////////////////////////////////////////////////////
 //                                                                        //
 // This file is run when you open root.                                   //
 // Just place it in your home directory and it works automatically.       // 
 // If you make a change to this file simply start                         //
 // a new root session and the changes will be implemented.                //
 //                                                                        //
 // If you did not install roofit then there will be an error              //
 // message when you login, it can be ignored.                             //     
 //                                                                        //
 ////////////////////////////////////////////////////////////////////////////

 // Definition of new styles
 TStyle *MuonG2_collaboration      = new TStyle("MuonG2_Collaboration","How MuonG2_Collaboration likes it");
 TStyle *MuonG2_collaboration_work = new TStyle("MuonG2_Collaboration_Work","How MuonG2_Collaboration likes it with stats");

 // Set the default parameters for style MuonG2_collaboration
 MuonG2_collaboration->SetCanvasBorderMode(0);     // Removes Canvas Border
 MuonG2_collaboration->SetPadBorderMode(0);        // Removes Pad Border
 MuonG2_collaboration->SetCanvasColor(0);          // Changes Canvas color to white
 MuonG2_collaboration->SetPadColor(0);             // Changes Pad color to white
 MuonG2_collaboration->SetStatColor(0);            // Changes Stats bg color to white
 MuonG2_collaboration->SetTitleFillColor(0);       // Changes Title bg color to white

 MuonG2_collaboration->SetFrameLineWidth(2);       // make frame pop 
 MuonG2_collaboration->SetLabelSize(0.05,"xyz");   // increase size for redability
 MuonG2_collaboration->SetTitleSize(0.05,"xyz");   // "
 MuonG2_collaboration->SetLabelFont(62,"xyz");     // helvetica bold for redability
 MuonG2_collaboration->SetTitleFont(62,"xyz");     // "

 MuonG2_collaboration->SetPadTickX(1);             // Sets tic marks on both horizontal axes
 MuonG2_collaboration->SetPadTickY(1);             // Sets tic marks on both vertical axes
 MuonG2_collaboration->SetTickLength(0.018,"xyz"); // Sets tic length
 MuonG2_collaboration->SetOptStat(0);              // Turns off Statistics display
 MuonG2_collaboration->SetOptTitle(0);             // Turns off Title display
 MuonG2_collaboration->SetHistLineWidth(2);        // Changes Histogram Line width
 MuonG2_collaboration->SetFrameBorderMode(0);      // Removes the Frame Border
 MuonG2_collaboration->SetFrameFillStyle(0);
 MuonG2_collaboration->SetCanvasDefH(494);         // Sets Default Canva Height
 MuonG2_collaboration->SetCanvasDefW(800);         // Sets Default Canvas Width
 MuonG2_collaboration->SetPalette(1);


 // The next for commands set the default margin size
 // n.b. the margins do not take axis labels into account! grr!!
 MuonG2_collaboration->SetPadTopMargin(0.1);     // Set Margin Top
 MuonG2_collaboration->SetPadRightMargin(0.1);   // Set Margin Right
 MuonG2_collaboration->SetPadBottomMargin(0.1);    // Set Margin Bottom
 MuonG2_collaboration->SetPadLeftMargin(0.1);     // Set Margin Left

 //make a nice palette
 int NRGBs = 7, NCont = 999;
 MuonG2_collaboration->SetNumberContours(NCont);
 Double_t stops[7] = { 0.00, 0.10, 0.25, 0.45, 0.60, 0.75, 1.00 };
 Double_t red[7]   = { 1.00, 0.00, 0.00, 0.00, 0.97, 0.97, 0.10 };
 Double_t green[7] = { 1.00, 0.97, 0.30, 0.40, 0.97, 0.00, 0.00 };
 Double_t blue[7]  = { 1.00, 0.97, 0.97, 0.00, 0.00, 0.00, 0.00 };
 TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);

 // Make MuonG2_collaboration_work a copy of MuonG2_collaboration and remove stats
 MuonG2_collaboration->Copy(*MuonG2_collaboration_work);
 MuonG2_collaboration_work->SetOptStat(1);
 MuonG2_collaboration_work->SetOptTitle(1);
 // MuonG2_collaboration_work->SetPadTopMargin(0.1);
 // MuonG2_collaboration_work->SetPadRightMargin(0.1);

 // added by aaron fienberg
 MuonG2_collaboration_work->SetMarkerStyle(20);
 // added after after root 6.12 update, axis labels flying off the canvas
 MuonG2_collaboration_work->SetTitleOffset(1,"xyz");   // "

 // Select Which Style to 
 gROOT->SetStyle("MuonG2_Collaboration_Work");
 // gROOT->SetStyle("MuonG2_Collaboration");


 // Use the RooFit functions
 // using namespace RooFit;

 // Force Root to run MuonG2_collaboration/MuonG2_collaboration_work style
 gROOT->ForceStyle();

}
