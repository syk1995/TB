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

using namespace std;
using namespace lcio;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input.slcio> <output.root>" << endl;
        return 1;
    }

    string slcioFile = argv[1];
    string outputFile = argv[2];
    LCReader* reader = LCFactory::getInstance()->createLCReader();
    reader->open(slcioFile);

    TFile* rootFile = new TFile(outputFile.c_str(), "RECREATE");
    TTree* tree = new TTree("tree", "Tree with collection data");

    int HCAL_cycleNr, HCAL_bunchXID, HCAL_eventNr, HCAL_chipID, HCAL_nChannels;
    vector<int> HCAL_tdc14bit, HCAL_adc14bit;

    int ECAL_cycleNr, ECAL_bunchXID, ECAL_sca, ECAL_layer, ECAL_skirocID, ECAL_nChannels;
    vector<int> ECAL_hit_low, ECAL_gain_low, ECAL_ADC_low, ECAL_hit_high, ECAL_gain_high, ECAL_ADC_high;

    tree->Branch("HCAL_CycleNr", &HCAL_cycleNr);
    tree->Branch("HCAL_BunchXID", &HCAL_bunchXID);
    tree->Branch("HCAL_EvtNr", &HCAL_eventNr);
    tree->Branch("HCAL_ChipID", &HCAL_chipID);
    tree->Branch("HCAL_NChannels", &HCAL_nChannels);
    tree->Branch("HCAL_TDC14bit", &HCAL_tdc14bit);
    tree->Branch("HCAL_ADC14bit", &HCAL_adc14bit);
    
    tree->Branch("ECAL_CycleNr", &ECAL_cycleNr);
    tree->Branch("ECAL_BunchXID", &ECAL_bunchXID);
    tree->Branch("ECAL_SCA", &ECAL_sca);
    tree->Branch("ECAL_Layer", &ECAL_layer);
    tree->Branch("ECAL_SkirocID", &ECAL_skirocID);
    tree->Branch("ECAL_NChannels", &ECAL_nChannels);
    tree->Branch("ECAL_Hit_Low", &ECAL_hit_low);
    tree->Branch("ECAL_Gain_Low", &ECAL_gain_low);
    tree->Branch("ECAL_ADC_Low", &ECAL_ADC_low);
    tree->Branch("ECAL_Hit_High", &ECAL_hit_high);
    tree->Branch("ECAL_Gain_High", &ECAL_gain_high);
    tree->Branch("ECAL_ADC_High", &ECAL_ADC_high);

    // 正确的事件读取方式
    LCEvent* event = nullptr;
    while ((event = reader->readNextEvent())) { 
        try {
            LCCollection* ecalcollection = event->getCollection("EUDAQDataSiECAL");

            for (int i = 0; i < ecalcollection->getNumberOfElements(); ++i) {
                LCGenericObject* obj = dynamic_cast<LCGenericObject*>(ecalcollection->getElementAt(i));

                if (!obj) continue;  

                // 读取ECAL数据
                ECAL_cycleNr = obj->getIntVal(0);
                ECAL_bunchXID = obj->getIntVal(1);
                ECAL_sca = obj->getIntVal(2);
                ECAL_layer = obj->getIntVal(3);
                ECAL_skirocID = obj->getIntVal(4);
                ECAL_nChannels = obj->getIntVal(5);

                // 清空之前的数据
                ECAL_hit_low.clear();
                ECAL_gain_low.clear();
                ECAL_ADC_low.clear();
                ECAL_hit_high.clear();
                ECAL_gain_high.clear();
                ECAL_ADC_high.clear();

                for (int j = 0; j < ECAL_nChannels; ++j) 
                {
                    int base = 6 + j * 6;  // 每个通道的信息紧挨着，每 6 个数据为一组
                    ECAL_hit_low.push_back(obj->getIntVal(base));  
                    ECAL_gain_low.push_back(obj->getIntVal(base + 1));  
                    ECAL_ADC_low.push_back(obj->getIntVal(base + 2));  
                    ECAL_hit_high.push_back(obj->getIntVal(base + 3));  
                    ECAL_gain_high.push_back(obj->getIntVal(base + 4));  
                    ECAL_ADC_high.push_back(obj->getIntVal(base + 5));  
                }


                tree->Fill();  
            }
        } catch (DataNotAvailableException& e) {
            cerr << "Collection 'EUDAQDataSiECAL' not found in this event." << endl;
        }

        try {
            LCCollection* hcalcollection = event->getCollection("EUDAQDataScCAL");

            for (int i = 0; i < hcalcollection->getNumberOfElements(); ++i) {
                LCGenericObject* obj = dynamic_cast<LCGenericObject*>(hcalcollection->getElementAt(i));

                if (!obj) continue;  
                // 读取HCAL数据
                HCAL_cycleNr = obj->getIntVal(0);
                HCAL_bunchXID = obj->getIntVal(1);
                HCAL_eventNr = obj->getIntVal(2);
                HCAL_chipID = obj->getIntVal(3);
                HCAL_nChannels = obj->getIntVal(4);

                // 清空之前的数据
                HCAL_tdc14bit.clear();
                HCAL_adc14bit.clear();
                for (int j = 0; j < HCAL_nChannels; ++j) {
                    HCAL_tdc14bit.push_back(obj->getIntVal(5 + j));  
                    HCAL_adc14bit.push_back(obj->getIntVal(5 + HCAL_nChannels + j));  
                }
                // cout << "i" << i << "HCAL cycleNr: " << HCAL_cycleNr << endl;
                tree->Fill();  
            }
        } catch (DataNotAvailableException& e) {
            cerr << "Collection 'EUDAQDataScCAL' not found in this event." << endl;
        }
    }

    rootFile->Write();
    rootFile->Close();
    reader->close();

    cout << "Data extraction and ROOT file creation completed." << endl;
    return 0;
}
