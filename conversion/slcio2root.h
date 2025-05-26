#ifndef SLCIO2ROOT_H
#define SLCIO2ROOT_H

#include <iostream>
#include <vector>
#include <lcio.h>
#include <IO/LCReader.h>
#include <IOIMPL/LCFactory.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/LCEvent.h>
#include <EVENT/LCIO.h>
#include <EVENT/LCGenericObject.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

#define NB_OF_SKIROCS_PER_ASU 16
#define NB_OF_CHANNELS_IN_SKIROC 64
#define NB_OF_SCAS_IN_SKIROC 15
#define SINGLE_SKIROC_EVENT_SIZE (129*2) 
#define SLBDEPTH 15
#define NB_CORE_DAUGHTERS 1 
#define NEGDATA_THR 11
#define BCIDTHRES 4

using namespace std;
using namespace lcio;
//Global
int cycle_current = 0;
int Evt_num=0;
int Evt_num_matched=0;
string branch_descriptor;
//HCAL
int HCAL_cycleNr, HCAL_bunchXID, HCAL_eventNr, HCAL_chipID, HCAL_nChannels;
vector<int> HCAL_tdc14bit, HCAL_adc14bit;
//ECAL
const int ECAL_LayerNo = 15;
const int ECAL_ChipNo = 16;
const int ECAL_SCANo = 15;
const int ECAL_EventMerge = 5;// Merge bcid difference<3 into a single event
int ECAL_cycleNr, ECAL_bunchXID, ECAL_sca, ECAL_layer, ECAL_skirocID, ECAL_nChannels,
    ECAL_hit_low, ECAL_hit_high;
std::map<int, std::vector<int>> ECAL_events;
std::vector<int> *ECAL_layer_ptr, *ECAL_chip_ptr, *ECAL_sca_ptr, *ECAL_channel_ptr,
    *ECAL_hitbit_low_ptr, *ECAL_gain_low_ptr, *ECAL_adc_low_ptr,
    *ECAL_hitbit_high_ptr, *ECAL_gain_high_ptr, *ECAL_adc_high_ptr;
int ECAL_corrected_bcid[ECAL_LayerNo][ECAL_ChipNo][ECAL_SCANo];
int ECAL_bcid_loop[ECAL_LayerNo][ECAL_ChipNo];
int ECAL_badbcid[ECAL_LayerNo][ECAL_ChipNo][ECAL_SCANo];
int ECAL_nhits[ECAL_LayerNo][ECAL_ChipNo][ECAL_SCANo];
int ECAL_entry[ECAL_LayerNo][ECAL_ChipNo][ECAL_SCANo];
bool b_empty; 
std::vector<int> ECAL_masked_chips={1109};//layer*100+chip
//BIF
std::map<int, int> BIF_events;//BCID and BIF TDC;
const double BIF_TDC2ns = 0.78125;//1 BIF TDC unit = 0.78125 ns
unsigned int BIF_acqNumber;
unsigned int  BIF_cycleNr;
long int BIF_Start;
long int BIF_End;
const long int BIF_TDC_Offset = 9192;
int BIF_bcid,BIF_TDC;




void ECAL_Clear();
void ECAL_ClearVectors();
void ECAL_ReadInt(LCGenericObject* objECAL);
void ECAL_ReadVector(LCGenericObject* objECAL);
std::map<int, std::vector<int>> ECAL_BuildEvents(LCCollection* ecalcollection, int ECAL_EventMerge);
std::map<int, std::vector<int>> mergeCloseBCIDs(const std::map<int, vector<int>>& merge_map, int threshold);
std::vector<int> mergeVectorsWithCheck(const std::vector<int>& v1, const std::vector<int>& v2);
std::map<int, int> BIF_BuildEvents(LCCollection* BIFcollection);
void GetBadBCID(int _badbcid[ECAL_LayerNo][ECAL_ChipNo][ECAL_SCANo], int _corrected_bcid[ECAL_LayerNo][ECAL_ChipNo][ECAL_SCANo], int _nhits[ECAL_LayerNo][ECAL_ChipNo][ECAL_SCANo]);
#endif // SLCIO2ROOT_H