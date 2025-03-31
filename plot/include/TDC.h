#include "ECAL.h"

std::string hist_name;
int hitbit_high, hitbit_low;
int layer, chip, sca, channel;
int adc_high, adc_low,TDC_ref;
double TDC_layer[Layer_No][5];//X, Y, Z, TDC,E
double X, Y, Z;

TH1F *h_TDC[Layer_No];
TH1F *h_TDC_ref[Layer_No];
TH2F *hitmap_high[Layer_No][2];//0 for chip-channel, 1 for xy
TH2F *h_TDC_E[Layer_No][2];//0 for TDC, 1 for TDC_ref

void TDC_Clear(){
    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        TDC_layer[i_layer][3]=-1;
    }
}

