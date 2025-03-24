#ifndef ECAL_H
#define ECAL_H

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

using namespace std;
const int Layer_No = 15;
const int Chip_No = 16;
const int SCA_No = 15;
const int Channel_No = 64;
const int Cell_No = 32;
const float CellSize=5.5;

class ECAL{
private:
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

    float Pos[Layer_No][Chip_No][Channel_No][3];
    float RangeX[2], RangeY[2];



public:
    ECAL(string Mapfile);
    ~ECAL();
    void Clear();
    int ReadDatalist(string datalist, vector<string>&runs, vector<string>&rootFiles);
    int ReadTree(TTree *tree);
    int HitMap(string datalist, string output);

};

#endif
