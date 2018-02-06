#include "style.h"
using namespace style;

void ReName(TString &name, TString tag, bool justTag = false)
{
  name.ReplaceAll("TAG", tag);
  if (justTag)
    return;

  name.ReplaceAll("()", "__");
  //name.ReplaceAll(".", "-");
  //name.ReplaceAll(":", "_VS_");
}

void draw_comparison(TString branch = "trackChargeFix.1501.7e5c17a")
{
  gstat(0);
  bool saveFigures = false;

  TString cut_goodVertex = "goodVertex";
  TString cut_good_no_distV = "goodVertex&&length_TAG>500&&numdEdx>20";
  TString cut_good = "goodVertex&&pocaV_TAG.Mag()<5&&length_TAG>500&&numdEdx>20";
  TString cut_good_light = "goodVertex&&pocaV_TAG.Mag()<10&&length_TAG>300&&numdEdx>10";
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
  //cuts.push_back(cut_goodVertex);    cutNames.push_back(TString("goodVertex"));
  cuts.push_back(cut_good_no_distV); cutNames.push_back(TString("withoutDistVCut"));
  //cuts.push_back(cut_good);          cutNames.push_back(TString("normalCut"));
  //cuts.push_back(cut_good_light);    cutNames.push_back(TString("ligthCut"));
  //cuts.push_back(cut_mpion);         cutNames.push_back(TString("mpion"));
  //cuts.push_back(cut_ppion);         cutNames.push_back(TString("ppion"));
  //cuts.push_back(cut_proton);        cutNames.push_back(TString("proton"));
  //cuts.push_back(cut_electron);      cutNames.push_back(TString("electron"));
  //cuts.push_back(cut_positron);      cutNames.push_back(TString("positron"));

  vector<TString> whatToPlot;
  whatToPlot.push_back(TString("pocaV_TAG.X()"));
  //whatToPlot.push_back(TString("pocaV_TAG.Y()"));
  //whatToPlot.push_back(TString("pocaV_TAG.Z()"));
  //whatToPlot.push_back(TString("pocaV_TAG.Mag()"));
  //whatToPlot.push_back(TString("pocaV_TAG.Perp()"));
  //whatToPlot.push_back(TString("pocaV2_TAG.Mag()"));
  //whatToPlot.push_back(TString("pocaV2_TAG.Perp()"));

  auto file = new TFile("data/summary.trackChargeFix.1501.7e5c17a.merged.root","read");
  auto tree = (TTree *) file -> Get("data");

  for (auto what : whatToPlot) {
    for (auto iCut = 0; iCut < cuts.size(); ++iCut) {
      auto cname = what+cutNames[iCut]; ReName(cname, "all");
      auto cvs = c(cname);
      auto lg = new TLegend();
      for (auto iTag = 0; iTag < 2; ++iTag) {
        auto tag = tags[iTag];
        auto hname = what + "_" + cutNames[iCut]; ReName(hname, tag);
        auto draw = what + ">>" + hname; ReName(draw, tag, true);
        auto cut = cuts[iCut]; ReName(cut, tag, true);
        auto entries = tree -> GetEntries(cut);
        cout << draw << " in " << cut << ":  " << entries << endl;

        auto h1 = make(new TH1D(hname,hname+";distance from vertex (mm);",500,0,2));
        h1 -> SetLineColor(colors[iTag]);
        h1 -> SetMinimum(0);
        lg -> AddEntry(h1,tag+Form(": %lld",entries),"l");
        tree -> Draw(draw,cut,options[iTag]);
      }
      make(lg) -> Draw();
      if (saveFigures) save(cvs,"png");
    }
  }
  return;

  vector<TString> whatToPlot2;
  whatToPlot2.push_back(TString("pocaV_TAG.Y():pocaV_TAG.X()"));
  whatToPlot2.push_back(TString("pocaV2_TAG.Y():pocaV2_TAG.X()"));

  for (auto what : whatToPlot2) {
    for (auto iCut = 0; iCut < cuts.size(); ++iCut) {
      for (auto iTag = 0; iTag < 2; ++iTag) {
        auto tag = tags[iTag];
        auto cname = what+cutNames[iCut]+"_"+tag; ReName(cname, "all");
        auto cvs = cc(cname);
        auto hname = what + "_" + cutNames[iCut]; ReName(hname, tag);
        auto draw = what + ">>" + hname; ReName(draw, tag, true);
        auto cut = cuts[iCut]; ReName(cut, tag, true);
        auto entries = tree -> GetEntries(cut);
        cout << draw << " in " << cut << ":  " << entries << endl;

        auto h2 = make(new TH2D(hname,hname+";dx (mm); dy (mm)",500,0,20,500,0,20));
        tree -> Draw(draw,cut,"colz");
        if (saveFigures) save(cvs,"png");
      }
    }
  }

  /*
  //auto file_cut = new TFile("data/copy_positive-pion.root","recreate");
  auto file_cut = new TFile("data/copy.root","recreate");
  file_cut -> cd(); auto tree_cut = tree -> CopyTree(cut);
  tree_cut -> Write();
  */
}
