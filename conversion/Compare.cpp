#include <iomanip>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <algorithm>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TString.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
const int Layer_No = 15;
const int Chip_No = 16;
const int SCA_No = 15;
const int Channel_No = 64;
const int Cell_No = 32;
const float CellSize=5.5;
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

int SetBranch(TTree *tree){
    tree->SetBranchAddress("bcid",_bcid);
    tree->SetBranchAddress("corrected_bcid",_corrected_bcid);
    tree->SetBranchAddress("badbcid",_badbcid);
    tree->SetBranchAddress("nhits",_nhits);
    tree->SetBranchAddress("adc_low",_adc_low);
    tree->SetBranchAddress("adc_high",_adc_high);
    tree->SetBranchAddress("autogainbit_low",_autogainbit_low);
    tree->SetBranchAddress("autogainbit_high",_autogainbit_high);
    tree->SetBranchAddress("hitbit_high",_hitbit_high);
    tree->SetBranchAddress("hitbit_low",_hitbit_low);
    return 1;
}


int Compare(){
    string inputFileName[2]={"/data_ilc/flc/shi/TB_2025_3/TB_Data/raw2root/raw_siwecal_90623.raw.root","/data_ilc/flc/shi/TB_2025_3/TB_Data/convertedfiles/eudaq_run_090623/eudaq_run_090623_raw.bin.root"};


    int bcid[Layer_No][Chip_No][SCA_No];
    int corrected_bcid[Layer_No][Chip_No][SCA_No];
    int badbcid[Layer_No][Chip_No][SCA_No];
    int nhits[Layer_No][Chip_No][SCA_No];
    int adc_low[Layer_No][Chip_No][SCA_No][Channel_No];
    int adc_high[Layer_No][Chip_No][SCA_No][Channel_No];
    int autogainbit_low[Layer_No][Chip_No][SCA_No][Channel_No];
    int autogainbit_high[Layer_No][Chip_No][SCA_No][Channel_No];
    int hitbit_low[Layer_No][Chip_No][SCA_No][Channel_No];
    int hitbit_high[Layer_No][Chip_No][SCA_No][Channel_No];


    TFile *inputFile[2];
    TTree *tree[2];
    for (int i = 0; i < 2; i++) {
        inputFile[i] = TFile::Open(inputFileName[i].c_str(), "READ");
        if (!inputFile[i] || inputFile[i]->IsZombie()) {
            std::cerr << "Error opening file: " << inputFileName[i] << std::endl;
            return 0;
        }

        tree[i] = (TTree*)inputFile[i]->Get("siwecaldecoded");
        if (!tree[i]) {
            std::cerr << "Error: Tree 'siwecaldecoded' not found in " << inputFileName[i] << std::endl;
            return 0;
        }
        SetBranch(tree[i]);
        std::cout << "Successfully opened " << inputFileName[i] << " and got tree 'siwecaldecoded'." << std::endl;
    }
    if((tree[0]->GetEntries())!=(tree[1]->GetEntries())){
        std::cerr << "Error: Number of events in two files are different." << std::endl;
        return 0;
    }
    //for (int i=0; i<tree[0]->GetEntries();i++) {
    for(int i_entry=0;i_entry<3;i_entry++) {
        cout<<i_entry<<endl;
        tree[0]->GetEntry(i_entry);
        for(int i_layer=0; i_layer < Layer_No;i_layer++ ){
            for(int i_chip=0; i_chip < Chip_No; i_chip++ ){
                for(int i_sca=0; i_sca < SCA_No; i_sca++ ){
                    bcid[i_layer][i_chip][i_sca] = _bcid[i_layer][i_chip][i_sca];
                    corrected_bcid[i_layer][i_chip][i_sca] = _corrected_bcid[i_layer][i_chip][i_sca];
                    badbcid[i_layer][i_chip][i_sca] = _badbcid[i_layer][i_chip][i_sca];
                    nhits[i_layer][i_chip][i_sca] = _nhits[i_layer][i_chip][i_sca];
                    for(int i_channel=0; i_channel < Channel_No; i_channel++){
                        adc_low[i_layer][i_chip][i_sca][i_channel] = _adc_low[i_layer][i_chip][i_sca][i_channel];
                        adc_high[i_layer][i_chip][i_sca][i_channel] = _adc_high[i_layer][i_chip][i_sca][i_channel];
                        autogainbit_low[i_layer][i_chip][i_sca][i_channel] = _autogainbit_low[i_layer][i_chip][i_sca][i_channel];
                        autogainbit_high[i_layer][i_chip][i_sca][i_channel] = _autogainbit_high[i_layer][i_chip][i_sca][i_channel];
                        hitbit_low[i_layer][i_chip][i_sca][i_channel] = _hitbit_low[i_layer][i_chip][i_sca][i_channel];
                        hitbit_high[i_layer][i_chip][i_sca][i_channel] = _hitbit_high[i_layer][i_chip][i_sca][i_channel];
                    }
                }
            }
        }
        tree[1]->GetEntry(i_entry);
        for(int i_layer=0; i_layer < Layer_No;i_layer++ ){
            for(int i_chip=0; i_chip < Chip_No; i_chip++ ){
                for(int i_sca=0; i_sca < SCA_No; i_sca++ ){
                    /*if(bcid[i_layer][i_chip][i_sca] != _bcid[i_layer][i_chip][i_sca]) cout<<i<<" "<<i_layer<<" "<<i_chip<<" "<<i_sca<<" "<<"different bcid"<<endl;
                    if(corrected_bcid[i_layer][i_chip][i_sca] != _corrected_bcid[i_layer][i_chip][i_sca]) cout<<i<<" "<<i_layer<<" "<<i_chip<<" "<<i_sca<<" "<<"different corrected bcid"<<endl;
                    if(badbcid[i_layer][i_chip][i_sca] != _badbcid[i_layer][i_chip][i_sca]) cout<<i<<" "<<i_layer<<" "<<i_chip<<" "<<i_sca<<" "<<"different bad bcid"<<endl;
                    if(nhits[i_layer][i_chip][i_sca] != _nhits[i_layer][i_chip][i_sca]) cout<<i<<" "<<i_layer<<" "<<i_chip<<" "<<i_sca<<" "<<"different nhits"<<endl;*/
                    for(int i_channel=0; i_channel < Channel_No; i_channel++){
                        /*if(adc_low[i_layer][i_chip][i_sca][i_channel] != _adc_low[i_layer][i_chip][i_sca][i_channel]) cout<<i<<" "<<i_layer<<" "<<i_chip<<" "<<i_sca<<" "<<i_channel<<" "<<"different adc_low"<<endl;
                        if(adc_high[i_layer][i_chip][i_sca][i_channel]
                            != _adc_high[i_layer][i_chip][i_sca][i_channel]) cout<<i<<" "<<i_layer<<" "<<i_chip<<" "<<i_sca<<" "<<i_channel<<" "<<"different adc_high"<<endl;
                        if(autogainbit_low[i_layer][i_chip][i_sca][i_channel]
                            != _autogainbit_low[i_layer][i_chip][i_sca][i_channel]) cout<<i<<" "<<i_layer<<" "<<i_chip<<" "<<i_sca<<" "<<i_channel<<" "<<"different autogainbit_low"<<endl;
                        if(autogainbit_high[i_layer][i_chip][i_sca][i_channel]
                            != _autogainbit_high[i_layer][i_chip][i_sca][i_channel]) cout<<i<<" "<<i_layer<<" "<<i_chip<<" "<<i_sca<<" "<<i_channel<<" "<<"different autogainbit_high"<<endl;
                        if(hitbit_low[i_layer][i_chip][i_sca][i_channel]
                            != _hitbit_low[i_layer][i_chip][i_sca][i_channel]) cout<<i<<" "<<i_layer<<" "<<i_chip<<" "<<i_sca<<" "<<i_channel<<" "<<"different hitbit_low"<<endl;
                        if(hitbit_high[i_layer][i_chip][i_sca][i_channel]
                            != _hitbit_high[i_layer][i_chip][i_sca][i_channel]) cout<<i<<" "<<i_layer<<" "<<i_chip<<" "<<i_sca<<" "<<i_channel<<" "<<"different hitbit_high "<<hitbit_high[i_layer][i_chip][i_sca][i_channel]<<" "<<_hitbit_high[i_layer][i_chip][i_sca][i_channel]<<endl;*/
                            if(adc_high[i_layer][i_chip][i_sca][i_channel]!= _adc_high[i_layer][i_chip][i_sca][i_channel]){cout<<i_entry<<" "<<i_layer<<" "<<i_chip<<" "<<i_sca<<" "<<i_channel<<" "<<"different adc_high "<<adc_high[i_layer][i_chip][i_sca][i_channel]<<" "<<_adc_high[i_layer][i_chip][i_sca][i_channel]<<endl;} 
                    }
                }
            }
        }
    }
    return 0;
}