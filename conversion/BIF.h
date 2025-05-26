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
int Evt_num=0;

//BIF
std::map<int, int> BIF_events;//BCID and BIF TDC;
const double BIF_TDC2ns = 0.78125;//1 BIF TDC unit = 0.78125 ns
unsigned int BIF_acqNumber,BIF_cycleNr;
long int BIF_Start,BIF_End;
int BIF_TDC,BIF_bcid;
const long int BIF_TDC_Offset = 0;//should be ~9192
std::vector<int> *BIF_TDC_v,*BIF_bcid_v;
//HCAL
std::vector<int> HCAL_events;//BCID
int HCAL_cycleNr, HCAL_bunchXID, HCAL_eventNr, HCAL_chipID, HCAL_nChannels;
vector<int> HCAL_tdc14bit, HCAL_adc14bit;

void BIF_Clear() {
    BIF_acqNumber = 0;
    BIF_cycleNr = 0;
    BIF_Start = 0;
    BIF_End = 0;
    BIF_bcid_v->clear();
    BIF_TDC_v->clear();
}
void HCAL_Clear() {
    HCAL_cycleNr = 0;
    HCAL_bunchXID = 0;
    HCAL_eventNr = 0;
    HCAL_chipID = 0;
    HCAL_nChannels = 0;
    HCAL_events.clear();
    HCAL_tdc14bit.clear();
    HCAL_adc14bit.clear();
}
std::map<int, int> BIF_BuildEvents(LCCollection* BIFcollection);
std::vector<int> HCAL_BuildEvents(LCCollection* hcalcollection);

#endif // SLCIO2ROOT_H