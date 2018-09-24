/**
 * \brief Export Single Histogram into ASCII file
 */
Bool_t SingleExportAscii(TH1* hist, TString &filename, TString folder="", TString separator="\t")
{
   Int_t i,j;
   Double_t xcenter, xwidth;
   Bool_t success=kFALSE;
   filename = folder + hist->GetName() + ".dat";
   ofstream file_out(filename);
   file_out << "# Output " << hist->ClassName() << ": " << hist->GetName() << " (" << hist->GetTitle() << ")\n";
   if (hist->GetDimension()==1)
   {
      file_out << "# BinCenter" << separator << "Content" << separator << "BinHalfWidth" << separator << "Error\n";
      for (i=1; i<=hist->GetNbinsX(); i++)
         file_out << hist->GetBinCenter(i) << separator << hist->GetBinContent(i) << separator << hist->GetBinWidth(i)/2 << separator << hist->GetBinError(i) << endl;
      if (i>1)
         success=kTRUE;
   }
   else if (hist->GetDimension()==2)
   {
      file_out << "# xBinCenter" << separator << "yBinCenter" << separator << "Content" << separator << "xBinHalfWidth" << separator << "yBinHalfWidth" << separator << "Error" << endl;
      for (i=1; i <= hist->GetNbinsX(); i++)
      {
         xcenter = hist->GetXaxis()->GetBinCenter(i);
         xwidth = hist->GetXaxis()->GetBinWidth(i)/2;
         for (j=1; j <= hist->GetNbinsY(); j++)
            file_out << xcenter << separator << hist->GetYaxis()->GetBinCenter(j) << separator << hist->GetBinContent(i,j) << separator << xwidth << separator << hist->GetYaxis()->GetBinWidth(j)/2 << separator << hist->GetBinError(i,j) << endl;
         if (j>1)
            file_out << endl; // produce a blank line after each set of Ybins for a certain Xbin. Gnuplot likes this.
      }
      if (i>1)
         success=kTRUE;
   }
   file_out.close();
   if (success == kTRUE)
      cout << "*** TRemiHistExport: Histogram " << hist->GetName() << " written to " << filename << endl;
   return success;
} 
