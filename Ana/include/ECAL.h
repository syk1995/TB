#ifndef ECAL_H
#define ECAL_H

#include <iomanip>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <map>
#include <algorithm>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TString.h>
#include <iostream>
#include <string>

const int Layer_No = 15;
const int Chip_No = 16;
const int SCA_No = 15;
const int Channel_No = 64;
const int Cell_No = 32;
const int BCID_No = 4096;
const float CellSize=5.5;

class ECAL{
private:
    std::string str_buffer;
    std::string treename_raw = "siwecaldecoded";
    std::string treename_selected = "siwecalselected";
    int _bcid[Layer_No][Chip_No][SCA_No];
    int _corrected_bcid[Layer_No][Chip_No][SCA_No];
    int _badbcid[Layer_No][Chip_No][SCA_No];
    int _nhits[Layer_No][Chip_No][SCA_No];
    int _adc_low[Layer_No][Chip_No][SCA_No][Channel_No];
    int _adc_high[Layer_No][Chip_No][SCA_No][Channel_No];
    int _autogainbit_low[Layer_No][Chip_No][SCA_No][Channel_No];
    int _autogainbit_high[Layer_No][Chip_No][SCA_No][Channel_No];
    int _hitbit_low[Layer_No][Chip_No][SCA_No][Channel_No];
    int _hitbit_high[Layer_No][Chip_No][SCA_No][Channel_No];
    //  int _event;
    //int numbcid;
    int _numCol[Layer_No][Chip_No];
    int _chipId[Layer_No][Chip_No];
    int _slot[Layer_No];
    int _slboard_id[Layer_No];
    int _n_slboards;
    int _acqNumber;
    //slowcontrol
    double _startACQ[Layer_No];
    int _rawTSD[Layer_No];
    float _TSD[Layer_No];
    int _rawAVDD0[Layer_No];
    int _rawAVDD1[Layer_No];
    float _AVDD0[Layer_No];
    float _AVDD1[Layer_No];
    //Mapping
    float Pos[Layer_No][Chip_No][Channel_No][3];
    float RangeX[2], RangeY[2];
    //Mask
    std::vector<int> _mask_channels;
    
    //IO vectors
    std::vector<int> *_layer_ptr;
    std::vector<int> *_chip_ptr;
    std::vector<int> *_sca_ptr;
    std::vector<int> *_channel_ptr;
    std::vector<int> *_adc_low_ptr;
    std::vector<int> *_adc_high_ptr;
    std::vector<int> *_autogainbit_low_ptr;
    std::vector<int> *_autogainbit_high_ptr;
    std::vector<int> *_hitbit_high_ptr;
    std::vector<int> *_hitbit_low_ptr;
    std::map<std::string, std::vector<int>*> BranchMap_int_v;
    std::map<std::string, int*> BranchMap_int;

    std::vector<std::string> BranchNames_raw = {
        "bcid",
        "corrected_bcid",
        "badbcid",
        "adc_low",
        "adc_high",
        "autogainbit_low",
        "autogainbit_high",
        "hitbit_high",
        "hitbit_low",
    };
    std::vector<std::string> BranchDesciptions_raw = {
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
    };
    std::vector<std::string> BranchNames_selected = {
        "bcid",
        "corrected_bcid",
        "badbcid",
        "adc_low",
        "adc_high",
        "autogainbit_low",
        "autogainbit_high",
        "hitbit_high",
        "hitbit_low",
    };
    std::vector<std::string> BranchDesciptions_selected = {
        "bcid[" + std::to_string(Layer_No) + "][" + std::to_string(Chip_No) + "][" + std::to_string(SCA_No) + "]/I",
        "corrected_bcid[" + std::to_string(Layer_No) + "][" + std::to_string(Chip_No) + "][" + std::to_string(SCA_No) + "]/I",
        "badbcid[" + std::to_string(Layer_No) + "][" + std::to_string(Chip_No) + "][" + std::to_string(SCA_No) + "]/I",
        "vector<int>*",
        "vector<int>*",
        "vector<int>*",
        "vector<int>*",
        "vector<int>*",
        "vector<int>*",
    };    

    std::vector<std::string> BranchNames_hitmap = {
        "bcid",
        "corrected_bcid",
        "layer",
        "chip",
        "sca",
        "channel",
        "adc_low",
        "adc_high",
        "hitbit_high",
        "hitbit_low",
    };
    std::vector<std::string> BranchDesciptions_hitmap = {
        "bcid[" + std::to_string(Layer_No) + "][" + std::to_string(Chip_No) + "][" + std::to_string(SCA_No) + "]/I",
        "corrected_bcid[" + std::to_string(Layer_No) + "][" + std::to_string(Chip_No) + "][" + std::to_string(SCA_No) + "]/I",
        "vector<int>*",
        "vector<int>*",
        "vector<int>*",
        "vector<int>*",
        "vector<int>*",
        "vector<int>*",
        "vector<int>*",
        "vector<int>*",
    };  

public:
    ECAL(std::string Mapfile,std::string Maskfile="");
    ~ECAL();
    void Clear();
    void Clear_Vector();
    int ReadDatalist(std::string datalist, std::vector<std::string>& runs, std::vector<std::string>& rootFiles);
    int ReadTree(TTree *tree, std::vector<std::string> BranchNames, std::vector<std::string> BranchDescriptors);
    int WriteTree(TTree *tree, std::vector<std::string> BranchNames, std::vector<std::string> BranchDescriptors);
    int HitMap(std::string datalist, std::string output);
    int Select(std::string datalist, std::string output);
    int TDC(std::string datalist, std::string output);
};

#endif
