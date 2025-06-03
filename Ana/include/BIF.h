#ifndef BIF_H
#define BIF_H
#include "ECAL.h"
#include "TGraph.h"

std::string string_buffer;
const int Read_EventNum=50000;
const int selected_layer=12;
const int Scanning_No = 501;
int curr_cycle;
//ECAL
int layer, chip, sca, channel,HitID,hitbit_high, hitbit_low,adc_high, adc_low;
int ECAL_curr_bcid;
//HCAL
std::vector<int> *HCAL_bcid_v;//BCID
int HCAL_cycleNr, HCAL_bunchXID, HCAL_eventNr, HCAL_chipID, HCAL_nChannels;
std::vector<int> HCAL_tdc14bit, HCAL_adc14bit;


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
TGraph *g_BIF_Offset, *g_HCAL_Offset;
class BIF : public ECAL{
protected:
    std::string treename_BIF = "BIF";
    const double _BIF_TDC2ns = 0.78125;//1 BIF TDC unit = 0.78125 ns
    int _BIF_acqNumber,_BIF_cycleNr;
    int _BIF_cycleNr_offset = -2;
    int _HCAL_cycleNr_offset = -3;
    int _BIF_TDC_Offset = 9192;
    int _BIF_bcid_Offset = 0;
    double X, Y, Z;
    int Matched_Events_BIF[Scanning_No], Matched_Events_HCAL[Scanning_No];  
    std::vector<int> *_BIF_TDC_v,*_BIF_bcid_v;
    std::vector<std::string> BranchNames_BIF = {
        "BIF_cycle",
        "BIF_bcid",
        "BIF_TDC",
        "HCAL_cycle",
        "HCAL_bcid",
    };
    std::vector<std::string> BranchDesciptions_BIF = {
        "BIF_cycle/I",
        "vector<int>*",
        "vector<int>*",
        "HCAL_cycle/I",
        "vector<int>*",
    };
public:
    BIF(std::string Mapfile,int BIF_cycleNr_offset=-2,int HCAL_cycleNr_offset=-3,std::string Maskfile=""); 
    ~BIF(){};
    void Clear();
    int ScanOffset(std::string datalist, std::string output);
    int EventBuild(std::string datalist, std::string output);
    int ReadDatalist(std::string datalist, std::vector<std::string>& runs, std::vector<std::string>& rootFiles, std::vector<std::string>& rootFiles_BIF);
    int Match_bcid(int *Matched_Events,int Scanning_No, std::map<int, std::vector<int>> &BIF_cycle_bcid_map, std::map<int, std::vector<int>> &ECAL_cycle_bcid_map);
    int Scan_BCIDOffset(int *Matched_Events, int Scanning_No, int offset, TTree *tree_BIF, TTree *tree_ECAL, std::string str_Type);
};

#endif