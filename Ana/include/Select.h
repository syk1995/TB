#include "ECAL.h"

int bcid_pre;
int bcid_loop[Layer_No][Chip_No];
int bcid[Layer_No][Chip_No][SCA_No];
int corrected_bcid[Layer_No][Chip_No][SCA_No];
int merge_delta=3;
bool b_empty;
std::map<int, std::vector<int>> bcid_map;//corrected bcid, 10000*layer+chip*100+sca
std::map<int, std::vector<int>> merged_bcid_map;
std::string hist_name;
TH1F *h_hitbit_high;
TH1F *h_hitbit_low;
TH1F *h_bcid;
TH1F *h_corrected_bcid;
TH1F *h_badbcid;
TH1F *h_bcid0;
TH1F *h_emptyEve_adc[Layer_No][2];
TH2F *h_emptyEve_Map[Layer_No][2];
TH2F *h_emptyEve_bcid_sca[Layer_No];
TH2F *h_emptyEve_chip_adc_high[Layer_No];
TH2F *h_emptyEve_chip_bcid[Layer_No];
TH2F *h_emptyEve_bcid_adc_high[Layer_No];

std::vector<int> maskchannel;
std::map<int, std::vector<int>> mergeCloseBCIDs(const std::map<int, std::vector<int>>& merge_map, int threshold = merge_delta);
std::vector<int> mergeVectorsWithCheck(const std::vector<int>& v1, const std::vector<int>& v2);
void Select_Clear(){
    bcid_map.clear();
    merged_bcid_map.clear();
    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        for(int i_chip=0; i_chip<Chip_No; i_chip++){
            bcid_loop[i_layer][i_chip]=0;
        }
    }
}



