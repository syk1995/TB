#include "ECAL.h"

std::string hist_name;
int hitbit_high, hitbit_low;
int layer, chip, sca, channel;
int adc_high, adc_low,TDC_ref,adc_max;
int binx, biny, binz,max_bin;
int Evt_num[Layer_No];
std::vector<int> adc_vec[Layer_No];
std::vector<int> ID_select={110316,120316};//layer*10000+chip*100+channel
int sca_pre;
int hitnum[Layer_No];
double TDC_layer[Layer_No][6];//X, Y, Z, TDC,E,bcid
double X, Y, Z;

bool b_MIP;
TH1F *h_pedestal[Layer_No];
TH1F *h_bcid[Layer_No];;
TH1F *h_ADC[Layer_No][2];//0 for all, 1 for selected
TH1F *h_TDC[Layer_No][2];//before and after event selection
TH1F *h_TDC_ref[Layer_No];
TH1F *h_hitnum[Layer_No];
TH2F *h_layer_hitnum;
TH2F *hitmap_high[Layer_No][2];//0 for chip-channel, 1 for xy
TH2F *h_TDC_E[Layer_No][2];//0 for TDC, 1 for TDC_ref

void TDC_Clear(){
    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        TDC_layer[i_layer][3]=-1;
        TDC_layer[i_layer][4]=-1;
        hitnum[i_layer]=0;
        sca_pre=-1;
        adc_vec[i_layer].clear();
    }
}

