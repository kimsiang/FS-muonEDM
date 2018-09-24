{
   TString  fileIn, fileOut;
   Double_t *r, *z, *Er, *Ez;
   Int_t    fileSize, rValues, zValues, index;
   
   fileIn  = "NosePlate_2DE.map";
   fileOut = "NosePlate_rawscaled_2DE.map";
   
   TGraphErrors *fileGraph = new TGraphErrors(fileIn);
    if ( fileGraph->IsZombie() ){
      cout << endl;
      cout << "File " << fileIn << " does not exist!!!" << endl << endl;
      return;
    }

    r        = fileGraph->GetX();
    z        = fileGraph->GetY();
    Er       = fileGraph->GetEX();
    Ez       = fileGraph->GetEY();
    fileSize = fileGraph->GetN();
    
    rValues  = r[0];
    zValues  = z[0];
    Double_t zScale[fileSize];
    Double_t EzScale[fileSize];
    
    zScale[0]  = z[0];
    EzScale[0] = Ez[0];
    for (Int_t i=1; i<fileSize; i++){
      zScale[i]  = z[i]*-1;
      EzScale[i] = Ez[i]*-1.;
    }
    
    FILE *fp = fopen(fileOut, "w");
    
    fprintf(fp,"%4.0f\t%4.0f\t mm\t0.00001000\n",rValues, zValues);
    for (Int_t j=0; j<rValues; j++){
     for (i=zValues; i>0; i--){
      index = j*zValues + i;
      fprintf(fp,"%4.0f\t%4.0f\t%10.3f\t%10.3f\n",r[index], zScale[index], Er[index], EzScale[index]);
     }
    }  
    fclose(fp); 
}