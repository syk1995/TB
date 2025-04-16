#include "ECAL.h"

std::string hist_name;
int hitbit_high, hitbit_low;
int layer, chip, sca, channel;
double X, Y, Z;
double adc_high, adc_low;
int EvtNum_layer[Layer_No];

TH1F *h_hitbit_high;
TH1F *h_hitbit_low;
TH1F *h_layerprofile;
TH1F *h_hitno_channel[Layer_No];
TH2F *h2_layerprofile;
TH2F *hitmap_high[Layer_No][2];//0 for chip-channel, 1 for xy
TH2F *hitmap_low[Layer_No][2];//0 for chip-channel, 1 for xy

void HitMap_Clear(){
    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        EvtNum_layer[i_layer]=0;
    }
}

