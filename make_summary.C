void make_summary()
{
  auto input_tree = new TChain("cbmsim");
  //TString tag = "trackChargeFix.1499.d50474b";  // before matching helix and reco track index
  //TString tag = "trackChargeFix.1500.8a7d00d";  // before format version fix
  //TString tag = "trackChargeFix.1501.7e5c17a";  // JungWoo - cut gg
  TString tag = "trackChargeFix.1502.9030344";  // PhysicsRun - use gg

  Int_t runs[] = {2900,2901,2902,2903,2904,2905};
  Int_t numEventsInSplit = 2000;
  Int_t numEventsInRun[] = {86916, 88750, 85788, 102451, 82746, 91334};
  Int_t numSplits[] = {44, 45, 43, 52, 42, 46};

  for (auto iRun = 0; iRun < 6; ++iRun)
    for (auto split = 0; split < numSplits[iRun]; ++split)
      input_tree -> Add(Form("data/run%d_s%d.reco.",runs[iRun],split)+tag+".root");

  TClonesArray *vertexArray = nullptr;
  TClonesArray *recoTrackArray = nullptr;
  TClonesArray *vaTrackArray = nullptr;

  input_tree -> SetBranchAddress("STVertex", &vertexArray);
  input_tree -> SetBranchAddress("STRecoTrack", &recoTrackArray);
  input_tree -> SetBranchAddress("VATracks", &vaTrackArray);

  auto fileName = TString("data/summary.")+tag+".root";
  TFile *output_file = new TFile(fileName,"recreate");
  TTree *output_tree = new TTree("data","");

  bool goodVertex;
  Int_t runID, splitID, eventID, entry, trackID, numTracks, vertexID;

  //reco
  Int_t trackCharge0, numdEdx0, pdg0;
  Double_t distVertex0, distVertex20, dedx0, trackLength0, c10, c20;
  TVector3 posVertex0, p0;

  //va
  Int_t trackCharge, numdEdx, pdg;
  Double_t distVertex, distVertex2, dedx, trackLength, c1, c2;
  TVector3 posVertex, p;

  output_tree -> Branch("runID",&runID);
  output_tree -> Branch("splitID",&splitID);
  output_tree -> Branch("eventID",&eventID);
  output_tree -> Branch("entry",&entry);
  output_tree -> Branch("trackID",&trackID);
  output_tree -> Branch("numTracks",&numTracks);
  output_tree -> Branch("vertexID",&vertexID);
  output_tree -> Branch("goodVertex",&goodVertex);
  output_tree -> Branch("posVertex","TVector3",&posVertex);

  output_tree -> Branch("p0",          "TVector3",&p0);
  output_tree -> Branch("dedx0",       &dedx0);
  output_tree -> Branch("trackCharge0",&trackCharge0);
  output_tree -> Branch("distVertex0", &distVertex0);
  output_tree -> Branch("pdg0",        &pdg0);
  output_tree -> Branch("numdEdx0",    &numdEdx0);
  output_tree -> Branch("trackLength0",&trackLength0);
  output_tree -> Branch("c10",         &c10);
  output_tree -> Branch("c20",         &c20);

  output_tree -> Branch("p",           "TVector3",&p);
  output_tree -> Branch("dedx",        &dedx);
  output_tree -> Branch("trackCharge", &trackCharge);
  output_tree -> Branch("distVertex",  &distVertex);
  output_tree -> Branch("pdg",         &pdg);
  output_tree -> Branch("numdEdx",     &numdEdx);
  output_tree -> Branch("trackLength", &trackLength);
  output_tree -> Branch("c1",          &c1);
  output_tree -> Branch("c2",          &c2);

  Int_t numEvents = input_tree -> GetEntries();
  for (entry = 0; entry < numEvents; ++entry)
  {
    if (entry % 1000 == 0)
      cout << "Event " << entry << " / " << numEvents << endl;

    input_tree -> GetEntry(entry);

    Int_t idxRun = 0;
    for (;idxRun < 6; ++idxRun) {
      auto tempID = eventID - numEventsInRun[idxRun];
      if (tempID < 0)
        break;
      else
        eventID = tempID;
    }
    runID = runs[idxRun];
    splitID = eventID/numEventsInSplit;
    eventID = eventID - numEventsInSplit*splitID;

    if (vertexArray -> GetEntries() != 1) continue;
    auto vertex = (STVertex *) vertexArray -> At(0);
    posVertex = vertex -> GetPos();

    goodVertex = false;
    if (posVertex.Z()<-9.49569&&posVertex.Z()>-12.80121&&posVertex.X()>-15&&posVertex.X()<15&&posVertex.Y()<-206.06&&posVertex.Y()>-246.06)
      goodVertex = true;

    numTracks = vaTrackArray -> GetEntries();
    for (trackID = 0; trackID < numTracks; ++trackID)
    {
      auto track0 = (STRecoTrack *) vaTrackArray -> At(trackID);
      auto track = (STRecoTrack *) vaTrackArray -> At(trackID);
      pdg0 = STPID::GetPDG(track0 -> GetPID());
      pdg = STPID::GetPDG(track -> GetPID());

      if (track -> GetVertexID() < 0) continue;
      vertexID = track -> GetVertexID();

      auto posPOCAVertex0 = track0 -> GetPOCAVertex();
      distVertex0 = sqrt(pow(posPOCAVertex0.X()-posVertex.X(),2)+pow(posPOCAVertex0.Y()-posVertex.Y(),2)+pow(posPOCAVertex0.Z()-posVertex.Z(),2));

      auto posPOCAVertex = track -> GetPOCAVertex();
      distVertex = sqrt(pow(posPOCAVertex.X()-posVertex.X(),2)+pow(posPOCAVertex.Y()-posVertex.Y(),2)+pow(posPOCAVertex.Z()-posVertex.Z(),2));
      //auto posTargetPlane = track->GetPosTargetPlane();
      //distVertex2 = sqrt(pow(posTargetPlane.X()-posVertex.X(),2)+pow(posTargetPlane.Y()-posVertex.Y(),2)+pow(posTargetPlane.Z()-posVertex.Z(),2));

      auto dedxArray0 = track0 -> GetdEdxPointArray();
      numdEdx0 = dedxArray0 -> size();

      if (numdEdx0 < 4) continue;
      auto dedxFirst = dedxArray0 -> at(0);
      auto dedxLast = dedxArray0 -> at(numdEdx-1);
      trackLength0 = dedxLast.fLength - dedxFirst.fLength;
      trackLength = dedxLast.fLength

      c10 = track0 -> GetEffCurvature1();
      c20 = track0 -> GetEffCurvature2();
      c1  = track  -> GetEffCurvature1();
      c2  = track  -> GetEffCurvature2();

      trackCharge0 = track0 -> GetCharge();
      trackCharge  = track  -> GetCharge();

      p0 = track0 -> GetMomentum();
      p  = track  -> GetMomentum();
      dedx0 = track0 -> GetdEdxWithCut(0,0.7);
      dedx  = track  -> GetdEdxWithCut(0,0.7);

      output_tree -> Fill();
    }
  }

  output_file -> cd();
  output_tree -> Write();
  cout << ">> " << fileName << endl;
}
