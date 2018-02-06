#include "style.h"
using namespace style;

void draw_comparison(TString branch = "trackChargeFix.1501.7e5c17a")
{
  gstat(0);
  bool saveFigures = false;

  TString cut_goodVertex = "goodVertex";
  TString cut_good = "goodVertex&&distVertex_TAG<5&&trackLength_TAG>500&&numdEdx0>20";
  TString cut_good_light = "goodVertex&&distVertex_TAG<10&&trackLength_TAG>300&&numdEdx0>10";
  TString cut_mpion = cut_good + "pdg==211&&trackCharge<0";
  TString cut_ppion = cut_good + "trackCharge>0&&p.Mag()<1000&&(dedx<-.1*(p.Mag()-250)+20||dedx<-.5*(p.Mag()-150)+40||dedx<20)&&p.Mag()<600";
  TString cut_proton = cut_good + "pdg==2212";
  TString cut_electron = cut_good + "p.Mag()<90&&dedx<30&&trackCharge<0";
  TString cut_positron = cut_good + "p.Mag()<90&&dedx<30&&trackCharge>0";

  //pocaV_reco.Mag();

  TString tags[] = {TString("vadd"), TString("reco")};
  TString options[] = {TString(""), TString("same")};
  Color_t colors[] = {kBlue, kPink};

  vector<TString> cuts;
  vector<TString> cutNames;
  cuts.push_back(cut_goodVertex); cutNames.push_back(TString("goodVertex"));
  //cuts.push_back(cut_good);       cutNames.push_back(TString("good"));
  //cuts.push_back(cut_good_light); cutNames.push_back(TString("good_light"));
  //cuts.push_back(cut_mpion);      cutNames.push_back(TString("mpion"));
  //cuts.push_back(cut_ppion);      cutNames.push_back(TString("ppion"));
  //cuts.push_back(cut_proton);     cutNames.push_back(TString("proton"));
  //cuts.push_back(cut_electron);   cutNames.push_back(TString("electron"));
  //cuts.push_back(cut_positron);   cutNames.push_back(TString("positron"));

  vector<TString> plotValues;
  //plotValues.push_back(TString("pocaV_TAG.Mag()"));
  //plotValues.push_back(TString("pocaV2_TAG.Mag()"));
  plotValues.push_back(TString("pocaV_TAG.Perp()"));
  plotValues.push_back(TString("pocaV2_TAG.Perp()"));

  auto file = new TFile("data/summary.trackChargeFix.1501.7e5c17a.merged.root","read");
  auto tree = (TTree *) file -> Get("data");

  for (auto plotValue : plotValues) {
    for (auto iCut = 0; iCut < cuts.size(); ++iCut) {
      auto cname = plotValue+cutNames[iCut]; cname.ReplaceAll("TAG","all"); cname.ReplaceAll("()","__"); cname.ReplaceAll(".","-");
      c(cname);
      auto lg = new TLegend();
      for (auto iTag = 0; iTag < 2; ++iTag) {
        auto tag = tags[iTag];
        auto hname = plotValue + "_" + cutNames[iCut]; hname.ReplaceAll("TAG",tag); hname.ReplaceAll("()","__"); hname.ReplaceAll(".","-");
        auto draw = plotValue + ">>" + hname; draw.ReplaceAll("TAG",tag);
        auto cut = cuts[iCut]; cut.ReplaceAll("TAG",tag);
        auto entries = tree -> GetEntries(cut);
        cout << draw << " in " << cut << ":  " << entries << endl;

        auto h1 = make(new TH1D(hname,";distance from vertex (mm);",200,0,20));
        h1 -> SetLineColor(colors[iTag]);
        lg -> AddEntry(h1,tag+Form(": %lld",entries),"l");
        tree -> Draw(draw,cut,options[iTag]);
      }
      make(lg) -> Draw();
      ++iCut;
    }
  }

  /*
  //auto file_cut = new TFile("data/copy_positive-pion.root","recreate");
  auto file_cut = new TFile("data/copy.root","recreate");
  file_cut -> cd(); auto tree_cut = tree -> CopyTree(cut);
  tree_cut -> Write();
  */
}
