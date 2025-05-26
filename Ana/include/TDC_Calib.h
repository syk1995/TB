#include "ECAL.h"
#include "cepcplotstyle.h"

const double Bunch_Crossing_Interval = 200; //ns
const int selected_layer = 12;
const int EntriesCut = 1000;
std::string hist_name;
std::string canvas_name;
int binx, biny, binz,max_bin;

TH1F *h_TDC[Layer_No][Chip_No][SCA_No][Channel_No][2];//0 for hittag=0, 1 for hittag=1
TH2F *h_Cycle_TDC[Layer_No][Chip_No][SCA_No][Channel_No][2];//0 for even bcid, 1 for odd bcid
TH1F *h_TDC_temp;
TH1F *h_TCCDiff[Layer_No][2];//0 for start, 1 for end
TH2F *h_hitmap[Layer_No][2];
TH2F *h2_TCC[Layer_No][2];//0 for chip channel, 1 for XY
TH2F *h_TCC_SCA[Layer_No];


double GetTCC(TH1F *h_TDC){
    if(h_TDC->GetEntries() < EntriesCut) return 0;
    h_TDC_temp = (TH1F*)h_TDC->Clone();
    h_TDC_temp->Rebin(64);
    double range_start = h_TDC_temp->GetBinLowEdge(h_TDC_temp->FindFirstBinAbove(10));
    double range_end = h_TDC_temp->GetBinLowEdge(h_TDC_temp->FindLastBinAbove(10))+h_TDC_temp->GetBinWidth(h_TDC_temp->FindLastBinAbove(10));
    h_TDC_temp->Clear();
    int bin_start = h_TDC->FindBin(range_start);
    int bin_end = h_TDC->FindBin(range_end);
    double TDC_start = h_TDC->GetBinCenter(h_TDC->FindFirstBinAbove(1,1, bin_start, bin_end));
    double TDC_end = h_TDC->GetBinCenter(h_TDC->FindLastBinAbove(1,1, bin_start, bin_end));     
    return (TDC_end-TDC_start)/Bunch_Crossing_Interval;
}




