#include "ECAL.h"

std::string hist_name;
int Read_EventNum=500000;
int hitbit_high, hitbit_low;
int layer, chip, sca, channel,HitID;
int adc_high, adc_low;;
int binx, biny, binz,max_bin;
int Evt_num[Layer_No];
std::vector<int> adc_vec[Layer_No];
//std::vector<int> selected_ID={110316,120316};//layer*10000+chip*100+channel
std::vector<int> selected_ID={120300,120301};//unhit channels
std::vector<int> selected_layers={8,11,12};
int reference_ID=110316;//-1 to mute this function
int hitnum[Layer_No];
int bcid_cur;
struct Hit{
    double X;
    double Y;
    double Z;
    double TDC;
    double ADC;
    int bcid;
    int sca;
    int ID;
};
std::vector<Hit> hit_vec;
double TDC_layer[Layer_No][8];//X, Y, Z, TDC,E,bcid,sca,ID
double Max_hit[5];//X, Y, Layer,TDC,ADC
double X, Y, Z,TDC_ref;

bool b_MIP;
TH1F *h_pedestal[Layer_No][2];//0 for low gain, 1 for high gain
TH2F *hitmap_high[Layer_No][2];//0 for chip-channel, 1 for xy
TH2F *h_BIF_TDC[Layer_No];
TH2F *h2_TDC_2channel;

void TDC_Clear(){
    TDC_ref=-1;
    for(int i=0; i<5; i++)Max_hit[i]=-1;
    bcid_cur=-1;
    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        TDC_layer[i_layer][3]=-1;
        TDC_layer[i_layer][4]=-1;
        hitnum[i_layer]=0;
        adc_vec[i_layer].clear();
    }
    hit_vec.clear();
}

