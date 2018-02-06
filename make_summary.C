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
  TClonesArray *vaddTrackArray = nullptr;

  input_tree -> SetBranchAddress("STVertex", &vertexArray);
  input_tree -> SetBranchAddress("STRecoTrack", &recoTrackArray);
  input_tree -> SetBranchAddress("VATracks", &vaddTrackArray);

  auto fileName = TString("data/summary.")+tag+".merged.root";
  TFile *output_file = new TFile(fileName,"recreate");
  TTree *output_tree = new TTree("data","");

  bool goodVertex;
  Int_t runID, splitID, eventID, entry, numTracks, vertexID, numdEdx;
  TVector3 posV;

  //reco
  Int_t charge_reco, pdg_reco, trackID_reco;
  Double_t distV_reco, distV2_reco, dedx_reco, length_reco;
  TVector3 p_reco, pocaV_reco, pocaV2_reco;

  //va
  Int_t charge_vadd, pdg_vadd, trackID_vadd;
  Double_t distV_vadd, distV2_vadd, dedx_vadd, length_vadd;
  TVector3 p_vadd, pocaV_vadd, pocaV2_vadd;

  output_tree -> Branch("runID",      &runID);
  output_tree -> Branch("splitID",    &splitID);
  output_tree -> Branch("eventID",    &eventID);
  output_tree -> Branch("numdEdx",    &numdEdx);
  output_tree -> Branch("goodVertex", &goodVertex);
  output_tree -> Branch("posV",       "TVector3", &posV);

  output_tree -> Branch("pdg_reco",     &pdg_reco);
  output_tree -> Branch("dedx_reco",    &dedx_reco);
  output_tree -> Branch("charge_reco",  &charge_reco);
  output_tree -> Branch("length_reco",  &length_reco);
  output_tree -> Branch("p_reco",       "TVector3", &p_reco);
  output_tree -> Branch("pocaV_reco",   "TVector3", &pocaV_reco);
  output_tree -> Branch("pocaV2_reco",  "TVector3", &pocaV2_reco);

  output_tree -> Branch("pdg_vadd",     &pdg_vadd);
  output_tree -> Branch("dedx_vadd",    &dedx_vadd);
  output_tree -> Branch("distV_vadd",   &distV_vadd);
  output_tree -> Branch("length_vadd",  &length_vadd);
  output_tree -> Branch("p_vadd",       "TVector3", &p_vadd);
  output_tree -> Branch("pocaV_vadd",   "TVector3", &pocaV_vadd);
  output_tree -> Branch("pocaV2_vadd",  "TVector3", &pocaV2_vadd);

  //output_tree -> Branch("entry",     &entry);
  //output_tree -> Branch("numTracks", &numTracks);
  //output_tree -> Branch("vertexID",  &vertexID);
  //output_tree -> Branch("trackID_reco", &trackID_reco);
  //output_tree -> Branch("trackID_vadd", &trackID_vadd);
  //output_tree -> Branch("distV_reco",   &distV_reco);
  //output_tree -> Branch("distV2_reco",  &distV2_reco);
  //output_tree -> Branch("distV2_vadd",  &distV2_vadd);
  //output_tree -> Branch("charge_vadd",  &charge_vadd);

  Int_t numEvents = input_tree -> GetEntries();
  for (entry = 0; entry < numEvents; ++entry)
  {
    if (entry % 1000 == 0)
      cout << "Event " << entry << " / " << numEvents << endl;

    input_tree -> GetEntry(entry);

    auto idxRun = 0;
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
    posV = vertex -> GetPos();

    goodVertex = false;
    if (posV.Z()<-9.49569&&posV.Z()>-12.80121&&posV.X()>-15&&posV.X()<15&&posV.Y()<-206.06&&posV.Y()>-246.06)
      goodVertex = true;

    auto firstTrack = (STRecoTrack *) vaddTrackArray -> At(0);
    if (firstTrack == nullptr)
      continue;

    vertexID = firstTrack -> GetVertexID();

    trackID_reco = 0;
    trackID_vadd = 0;

    numTracks = recoTrackArray -> GetEntries();
    if (numTracks != vaddTrackArray -> GetEntries())
      continue;

    for (; trackID_reco < numTracks; ++trackID_reco)
    {
      trackID_vadd = trackID_reco; //XXX
      auto track_reco = (STRecoTrack *) recoTrackArray -> At(trackID_reco);
      auto track_vadd = (STRecoTrack *) vaddTrackArray -> At(trackID_vadd);

      auto dedxArray0 = track_reco -> GetdEdxPointArray();
      numdEdx = dedxArray0 -> size();
      if (numdEdx < 4)
        continue;

      pdg_reco = STPID::GetPDG(track_reco -> GetPID());
      pdg_vadd  = STPID::GetPDG(track_vadd  -> GetPID());

      pocaV_reco = track_reco -> GetPOCAVertex() - posV;      //distV_reco = pocaV_reco.Mag();
      pocaV_vadd = track_vadd -> GetPOCAVertex() - posV;      //distV_vadd = pocaV_vadd.Mag();
      pocaV2_reco = track_reco -> GetPosTargetPlane() - posV; //distV2_vadd = sqrt(pocaV2_reco.X()*pocaV2_reco.X() + pocaV2_reco.Y()*pocaV2_reco.Y());
      pocaV2_vadd = track_vadd -> GetPosTargetPlane() - posV; //distV2_vadd = sqrt(pocaV2_vadd.X()*pocaV2_vadd.X() + pocaV2_vadd.Y()*pocaV2_vadd.Y());

      auto dedxFirst = dedxArray0 -> at(0);
      auto dedxLast = dedxArray0 -> at(numdEdx-1);
      length_reco = dedxLast.fLength - dedxFirst.fLength;
      length_vadd = dedxLast.fLength;

      charge_reco = track_reco -> GetCharge();
      charge_vadd  = track_vadd  -> GetCharge();

      p_reco = track_reco -> GetMomentum();
      p_vadd = track_vadd -> GetMomentum();
      dedx_reco = track_reco -> GetdEdxWithCut(0,0.7);
      dedx_vadd = track_vadd -> GetdEdxWithCut(0,0.7);

      output_tree -> Fill();
    }
  }

  output_file -> cd();
  output_tree -> Write();
  cout << ">> " << fileName << endl;
}
