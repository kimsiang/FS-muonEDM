{
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

  for (Int_t i=0; i<100; i++) {
    hDettLeft->Fill(det_time_start[j]);
    hDetz->Fill(det_z[i]);
    hEventID->Fill(eventID);
  }
}

