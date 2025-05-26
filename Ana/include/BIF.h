#ifndef BIF_H
#define BIF_H
#include "ECAL.h"
#include "TGraph.h"

std::string string_buffer;
const int Read_EventNum=50000000;
const int selected_layer=12;
const int Scanning_No = 2001;
int layer, chip, sca, channel,HitID,hitbit_high, hitbit_low,adc_high, adc_low;
int ECAL_curr_bcid;
int curr_cycle;



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

TH1F *h_TDC[Layer_No][Chip_No][Channel_No];
TH2F *h_hitmap[Layer_No][2];//0: ChipChannel, 1: XY
TGraph *g_BIF_Offset;
class BIF : public ECAL{
protected:
    std::string treename_BIF = "BIF";
    const double _BIF_TDC2ns = 0.78125;//1 BIF TDC unit = 0.78125 ns
    int _BIF_acqNumber,_BIF_cycleNr;
    const int _BIF_cycleNr_offset = 1;
    int _BIF_TDC_Offset = 9192;
    int _BIF_bcid_Offset = 0;
    double X, Y, Z;
    int Matched_Events[Scanning_No];
    std::map<int, std::vector<int>> BIF_cycle_bcid_map; //cycleNr -> bcid vector
    std::map<int, std::vector<int>> ECAL_cycle_bcid_map; //cycleNr -> bcid vector  
    std::vector<int> *_BIF_TDC_v,*_BIF_bcid_v;
    std::vector<std::string> BranchNames_BIF = {
        "BIF_cycle",
        "BIF_bcid",
        "BIF_TDC",
    };
    std::vector<std::string> BranchDesciptions_BIF = {
        "BIF_cycle/I",
        "vector<int>*",
        "vector<int>*",
    };

public:
    BIF(std::string Mapfile,std::string Maskfile=""); 
    ~BIF(){};
    void Clear();
    int EventBuild(std::string datalist, std::string output);
    int ReadDatalist(std::string datalist, std::vector<std::string>& runs, std::vector<std::string>& rootFiles, std::vector<std::string>& rootFiles_BIF);
    int Match_bcid(int &Matched_Events[Scanning_No], std::map<int, std::vector<int>> &BIF_cycle_bcid_map, std::map<int, std::vector<int>> &ECAL_cycle_bcid_map);
};

#endif