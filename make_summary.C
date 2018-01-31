void make_summary()
{
  auto input_tree = new TChain("cbmsim");
  //TString tag = "trackChargeFix.1499.d50474b";  // before matching helix and reco track index
  //TString tag = "trackChargeFix.1500.8a7d00d";  // before format version fix
  TString tag = "trackChargeFix.1501.7e5c17a";  // JungWoo - cut gg
  //TString tag = "trackChargeFix.1502.9030344";  // PhysicsRun - use gg

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
  input_tree -> SetBranchAddress("STVATrack", &vaTrackArray);

  TFile *output_file = new TFile(TString("data/summary.")+tag+".root","recreate");
  TTree *output_tree = new TTree("data","");

  bool goodVertex;
  Int_t runID, splitID, eventID, entry, trackID, numTracks, pdg, vertexID, trackCharge, numdEdx;
  Double_t distVertex, distVertex2, dedx, trackLength, effCurvature1, effCurvature2, effCurvature3;
  TVector3 posVertex, p;

  output_tree -> Branch("runID",&runID);
  output_tree -> Branch("splitID",&splitID);
  output_tree -> Branch("eventID",&eventID);

  output_tree -> Branch("entry",&entry);
  output_tree -> Branch("numTracks",&numTracks);
  output_tree -> Branch("goodVertex",&goodVertex);
  output_tree -> Branch("posVertex","TVector3",&posVertex);

  output_tree -> Branch("trackID",&trackID);
  output_tree -> Branch("p","TVector3", &p);
  output_tree -> Branch("dedx",&dedx);
  output_tree -> Branch("trackCharge",&trackCharge);

  output_tree -> Branch("vertexID",&vertexID);
  output_tree -> Branch("distVertex",&distVertex);
  output_tree -> Branch("distVertex2",&distVertex2);

  output_tree -> Branch("pdg",&pdg);
  output_tree -> Branch("numdEdx",&numdEdx);
  output_tree -> Branch("trackLength",&trackLength);
  output_tree -> Branch("c1",&effCurvature1);
  output_tree -> Branch("c2",&effCurvature2);
  output_tree -> Branch("c3",&effCurvature3);

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

    numTracks = recoTrackArray -> GetEntries();
    for (trackID = 0; trackID < numTracks; ++trackID)
    {
      auto track = (STRecoTrack *) recoTrackArray -> At(trackID);
      auto pid = track -> GetPID();
      pdg = STPID::GetPDG(pid);

      if (track -> GetVertexID() < 0) continue;
      vertexID = track -> GetVertexID();

      auto posPOCAVertex = track -> GetPOCAVertex();
      distVertex = sqrt(pow(posPOCAVertex.X()-posVertex.X(),2)+pow(posPOCAVertex.Y()-posVertex.Y(),2)+pow(posPOCAVertex.Z()-posVertex.Z(),2));

      auto posTargetPlane = track->GetPosTargetPlane();
      distVertex2 = sqrt(pow(posTargetPlane.X()-posVertex.X(),2)+pow(posTargetPlane.Y()-posVertex.Y(),2)+pow(posTargetPlane.Z()-posVertex.Z(),2));

      auto dedxArray = track -> GetdEdxPointArray();
      numdEdx = dedxArray -> size();
      if (numdEdx < 4) continue;
      auto dedxLast = dedxArray -> at(numdEdx-1);
      trackLength = dedxLast.fLength;

      effCurvature1 = track -> GetEffCurvature1();
      effCurvature2 = track -> GetEffCurvature2();
      effCurvature3 = track -> GetEffCurvature3();

      trackCharge = track -> GetCharge();

      p = track -> GetMomentum();
      dedx = track -> GetdEdxWithCut(0,0.7);

      output_tree -> Fill();
    }
  }

  output_file -> cd();
  output_tree -> Write();
}
