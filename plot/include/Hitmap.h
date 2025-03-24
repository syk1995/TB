#include "ECAL.h"

string hist_name;
TH1F *h_hitbit_high;
TH1F *h_hitbit_low;
TH2F *hitmap_high[Layer_No][2];//0 for chip-channel, 1 for xy
TH2F *hitmap_low[Layer_No][2];//0 for chip-channel, 1 for xy


