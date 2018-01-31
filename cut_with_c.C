#include "style.h"
using namespace style;

void cut_with_c()
{
  ifstream data("data/cut_percentage.dat");
  Int_t n;
  Double_t p1, p2;
  Int_t colors[3] = {kBlack,kPink,kBlue};

  gstat(0);

  auto cvs = c();
  cvs -> SetGrid();

  auto hist = new TH1D("hist","for (-)charged;not-#pi ratio (CUT);#pi ratio (LOSS)",100,0.6,1);
  hist -> SetMaximum(0.8);
  make(hist) -> Draw();
  auto legend = new TLegend();
  TGraph *graphc[3];
  for (auto c = 0; c < 3; ++c) {
    graphc[c] = new TGraph();
    auto gc = graphc[c];
    gc -> SetMarkerColor(colors[c]);
    gc -> SetLineColor(colors[c]);

    data >> n;
    for (auto i = 0; i < n; ++i) {
      data >> p1 >> p2;
      gc -> SetPoint(gc->GetN(),p2,p1);
    }

    make(gc) -> Draw("lpsame");
    legend -> AddEntry(gc,Form("C%d",c+1),"pl");
  }
  make(legend) -> Draw();

  save(cvs,"png");
}
