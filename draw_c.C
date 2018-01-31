#include "style.h"
using namespace style;

void draw_c()
{
  gstat(0);
  auto file1 = new TFile("data/copy_notpion.root","read");
  auto tree1 = (TTree *) file1 -> Get("data");
  auto hist_prot_c1 = new TH1D("hist_prot_c1","C1",50,-500,500);
  auto hist_prot_c2 = new TH1D("hist_prot_c2","C2",50,-10 ,10);
  auto hist_prot_c3 = new TH1D("hist_prot_c3","C3",50,-10 ,10);
  tree1 -> Project("hist_prot_c1","-effCurvature1","" ,"");
  tree1 -> Project("hist_prot_c2","-effCurvature2","" ,"");
  tree1 -> Project("hist_prot_c3"," effCurvature3","" ,"");
  hist_prot_c1 -> SetLineColor(kBlue);
  hist_prot_c2 -> SetLineColor(kBlue);
  hist_prot_c3 -> SetLineColor(kBlue);

  auto file2 = new TFile("data/copy_negativepion.root","read");
  auto tree2 = (TTree *) file2 -> Get("data");
  auto hist_pion_c1 = new TH1D("hist_pion_c1","C1",50,-500,500);
  auto hist_pion_c2 = new TH1D("hist_pion_c2","C2",50,-10 ,10);
  auto hist_pion_c3 = new TH1D("hist_pion_c3","C3",50,-10 ,10);
  tree2 -> Project("hist_pion_c1","-effCurvature1","" ,"");
  tree2 -> Project("hist_pion_c2","-effCurvature2","" ,"");
  tree2 -> Project("hist_pion_c3"," effCurvature3","" ,"");
  hist_pion_c1 -> SetLineColor(kRed);
  hist_pion_c2 -> SetLineColor(kRed);
  hist_pion_c3 -> SetLineColor(kRed);

  auto cvs1 = c("compare_c1");
  //cvs1 -> SetLogy();
  auto hpr1 = make(hist_prot_c1) -> DrawNormalized("",1);
  auto hpi1 = make(hist_pion_c1) -> DrawNormalized("same",1);
  cout << "================ 1" << endl;
  for (auto bin : {25,24,23,22,21,20,19,18,17,16,15,14,13,12})
    cout << bin << ": " << setw(12) << hpi1 -> Integral(bin,25) << " " << setw(12) << hpr1 -> Integral(bin,25) << endl;
  auto l1 = new TLegend();
  l1->AddEntry(hist_prot_c1,"not pion normalized");
  l1->AddEntry(hist_pion_c1,"(-) pion normalized");
  make(l1) -> Draw();
  //save(cvs1,"png");

  auto cvs2 = c("compare_c2");
  //cvs2 -> SetLogy();
  hpr1 = make(hist_prot_c2) -> DrawNormalized("",1);
  hpi1 = make(hist_pion_c2) -> DrawNormalized("same",1);
  cout << "================ 2" << endl;
  for (auto bin : {25,24,23,22,21,20,19,18,17,16,15,14,13,12})
    cout << bin << ": " << setw(12) << hpi1 -> Integral(bin,25) << " " << setw(12) << hpr1 -> Integral(bin,25) << endl;
  auto l2 = new TLegend();
  l2->AddEntry(hist_prot_c2,"not pion normalized");
  l2->AddEntry(hist_pion_c2,"(-) pion normalized");
  make(l2) -> Draw();
  //save(cvs2,"png");

  auto cvs3 = c("compare_c3");
  //cvs3 -> SetLogy();
  hpr1 = make(hist_prot_c3) -> DrawNormalized("",1);
  hpi1 = make(hist_pion_c3) -> DrawNormalized("same",1);
  cout << "================ 3" << endl;
  for (auto bin : {25,24,23,22,21,20,19,18,17,16,15,14,13,12})
    cout << bin << ": " << setw(12) << hpi1 -> Integral(bin,25) << " " << setw(12) << hpr1 -> Integral(bin,25) << endl;
  auto l3 = new TLegend();
  l3->AddEntry(hist_prot_c3,"not pion normalized");
  l3->AddEntry(hist_pion_c3,"(-) pion normalized");
  make(l3) -> Draw();
  //save(cvs3,"png");
}
