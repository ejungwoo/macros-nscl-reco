#include "style.h"
using namespace style;

void draw_plots()
{
  gstat(0);
  bool save_figures = false;

  auto file = new TFile("data/summary.root");
  //auto file = new TFile("data/summary.trackChargeFix.1499.d50474b.root");
  auto tree = (TTree *) file -> Get("data");

  TCut cut_good = "goodVertex && distVertex<10 && trackLength>500 && numdEdx>10";
  TCut cut_good_pion = cut_good + "pdg==211";
  TCut cut_good_proton = cut_good + "pdg==2212";
  TCut cut_good_antiproton = cut_good_proton + "trackCharge<0";
  TCut cut_good_antiproton2 = cut_good + "pdg!=211&&trackCharge<0";
  TCut cut_good_positivepion = "trackCharge>0&&p<1000&&(dedx<-.1*(p-250)+20||dedx<-.5*(p-150)+40||dedx<20)&&p<600";

  //TCut cut = cut_good + "abs(effCurvature2)>3";
  TCut cut = cut_good;
  //TCut cut = cut_good_antiproton2 + "p>-600";
  //TCut cut = cut_good_pion + "trackCharge<0";
  cout << "Cut is: " << cut << endl;

  /*
  //auto file_cut = new TFile("data/copy_positive-pion.root","recreate");
  auto file_cut = new TFile("data/copy.root","recreate");
  file_cut -> cd();
  auto tree_cut = tree -> CopyTree(cut);
  tree_cut -> Write();
  */

  gStyle -> SetPalette(kRainBow);
  TCanvas *cvs;

  cvs = cc();
  //auto h = make(new TH2D("pid",";p (MeV/c); dE/dx (ADC/mm);",500,-1500,3000,500,0,1000));
  auto h = make(new TH2D("pid",";p (MeV/c); dE/dx (ADC/mm);",500,-800,2000,500,0,1000));
  //h -> GetXaxis() -> SetNdivisions(505);
  tree -> Draw("dedx:p>>pid",cut,"colz");
  cvs -> SetLogz();
  if (save_figures) cvs -> SaveAs(TString("figures/goodcut_")+cvs->GetName()+".png");
  return;

  cvs = c();
  make(new TH1D("ec1","C1",200,-5000,10000));
  tree -> Draw("-effCurvature1>>ec1",cut);
  if (save_figures) cvs -> SaveAs(TString("figures/goodcut_")+cvs->GetName()+".png");

  cvs = c();
  make(new TH1D("ec2","C2",200,-50,100));
  tree -> Draw("-effCurvature2>>ec2",cut);
  if (save_figures) cvs -> SaveAs(TString("figures/goodcut_")+cvs->GetName()+".png");
  
  cvs = c();
  make(new TH1D("ec3","C3",200,-100,200));
  tree -> Draw("effCurvature3>>ec3",cut);
  if (save_figures) cvs -> SaveAs(TString("figures/goodcut_")+cvs->GetName()+".png");

  cvs = cc();
  make(new TH2D("ec12","C1 vs C2",200,-5000,10000, 200,-50,100));
  tree -> Draw("-effCurvature2:-effCurvature1>>ec12",cut,"colz");
  if (save_figures) cvs -> SaveAs(TString("figures/goodcut_")+cvs->GetName()+".png");

  cvs = cc();
  make(new TH2D("ec13","C1 vs C3",200,-5000,10000, 200,-100,200));
  tree -> Draw("effCurvature3:-effCurvature1>>ec13",cut,"colz");
  if (save_figures) cvs -> SaveAs(TString("figures/goodcut_")+cvs->GetName()+".png");
}
