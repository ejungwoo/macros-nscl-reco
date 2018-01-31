#include <fstream>
using namespace std;

void create_eventList()
{
  //auto file = new TFile("data/copy_negativepion.root");
  //auto file = new TFile("data/copy_highp-negative-pion.root");
  //auto file = new TFile("data/copy_positive-pion.root");
  auto file = new TFile("data/copy_notpion.root");
  auto tree = (TTree *) file -> Get("data");
  Int_t eventID = 0;
  Int_t trackID = 0;
  tree -> SetBranchAddress("eventID",&eventID);
  tree -> SetBranchAddress("trackID",&trackID);

  Int_t runs[] = {2900,2901,2902,2903,2904,2905};
  Int_t numEventsInRuns[] = {86916, 88750, 85788, 102451, 82746, 91334};
  Int_t numEventsInSplit = 2000;

  ofstream runlist("eventList.anti-proton3.txt");

  auto n = tree -> GetEntries();
  for (auto entry = 0; entry < n; ++entry) {
    tree -> GetEntry(entry);

    Int_t iRun = 0;
    for (;iRun < 6; ++iRun) {
      if (eventID - numEventsInRuns[iRun] < 0)
        break;
      else
        eventID = eventID - numEventsInRuns[iRun];
    }
    Int_t runID = runs[iRun];
    Int_t split = eventID/numEventsInSplit;
    eventID = eventID - numEventsInSplit*split;

    runlist << runID << " " << split << " " << eventID << " " << trackID << endl;
  }
}
