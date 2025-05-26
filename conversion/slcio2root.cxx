
#include "slcio2root.h"
#include <chrono>
using namespace std;
using namespace lcio;

int slcio2root(string inputFile, string outputFile) {
    auto start = std::chrono::high_resolution_clock::now();
    //Initialize output root and tree
    #pragma region
    TFile* rootFile = new TFile(outputFile.c_str(), "RECREATE");
    if (!rootFile->IsOpen()) {
        cerr << "Error: Unable to open output ROOT file "<<outputFile << endl;
        return -1;
    }
    TTree* tree = new TTree("ECAL_TDCEvents", "ECAL TDC data + BIF data");
    ECAL_layer_ptr = new std::vector<int>;
    ECAL_chip_ptr = new std::vector<int>;
    ECAL_sca_ptr = new std::vector<int>;
    ECAL_channel_ptr = new std::vector<int>;
    ECAL_adc_high_ptr = new std::vector<int>;
    ECAL_adc_low_ptr = new std::vector<int>;
    ECAL_hitbit_high_ptr = new std::vector<int>;
    ECAL_hitbit_low_ptr = new std::vector<int>;
    ECAL_gain_high_ptr = new std::vector<int>;
    ECAL_gain_low_ptr = new std::vector<int>;
    for (int i = 0; i < ECAL_LayerNo; ++i) {
        for (int j = 0; j < ECAL_ChipNo; ++j) {
            for (int k = 0; k < ECAL_SCANo; ++k) {
                ECAL_corrected_bcid[i][j][k] = 0;
            }
        }
    }
    tree->Branch("acqNumber", &ECAL_cycleNr, "acqNumber/I");
    branch_descriptor = "corrected_bcid[" + std::to_string(ECAL_LayerNo) + "]["+ std::to_string(ECAL_ChipNo) + "]["+ std::to_string(ECAL_SCANo) + "]/I";
    tree->Branch("corrected_bcid", (int*)ECAL_corrected_bcid, branch_descriptor.c_str());
    tree->Branch("layer", ECAL_layer_ptr);
    tree->Branch("chip", ECAL_chip_ptr);
    tree->Branch("sca", ECAL_sca_ptr);
    tree->Branch("channel", ECAL_channel_ptr);
    tree->Branch("adc_low", ECAL_adc_low_ptr);
    tree->Branch("adc_high", ECAL_adc_high_ptr);
    tree->Branch("hitbit_high", ECAL_hitbit_high_ptr);
    tree->Branch("hitbit_low", ECAL_hitbit_low_ptr);
    tree->Branch("gain_high", ECAL_gain_high_ptr);
    tree->Branch("gain_low", ECAL_gain_low_ptr);
    tree->Branch("BIF_bcid", &BIF_bcid, "BIF_bcid/I");
    tree->Branch("BIF_TDC", &BIF_TDC, "BIF_TDC/I");
    #pragma endregion

    LCReader* reader = LCFactory::getInstance()->createLCReader();
    LCCollection* ecalcollection = nullptr;
    LCCollection* BIFcollection = nullptr;
    LCGenericObject* objBIF = nullptr;
    LCGenericObject* objECAL = nullptr;
    LCEvent* event = nullptr;
    
    cout<<"Read "<<inputFile<<endl;
    try {
        reader->open(inputFile);
        std::cout << "File opened successfully." << std::endl;
    } catch (lcio::IOException& e) {
        std::cerr << "Error opening file: " << e.what() << std::endl;
        return -1;
    }
    for(int i = 0; (event = reader->readNextEvent()) && i<5; i++){
        if(i%1==0)cout << "Processing event " << i << endl;
        try{
            ecalcollection = event->getCollection("EUDAQDataSiECAL");
            BIFcollection = event->getCollection("EUDAQDataBIF");
            ECAL_events = ECAL_BuildEvents(ecalcollection, ECAL_EventMerge);
            BIF_events = BIF_BuildEvents(BIFcollection);
            Evt_num+=ECAL_events.size();
            cout<<"ECAL events: "<<ECAL_events.size()<<" BIF events: "<<BIF_events.size()<<endl;
            for (auto it = ECAL_events.begin(); it != ECAL_events.end(); ++it){
                ECAL_Clear();
                bool bcid_matched = false;
                int bcid_cur=it->first;
                for (int i_element : it->second) {
                    objECAL = dynamic_cast<LCGenericObject*>(ecalcollection->getElementAt(i_element));
                    if (!objECAL) continue;
                    ECAL_ReadInt(objECAL);
                    ECAL_ReadVector(objECAL);
                    ECAL_corrected_bcid[ECAL_layer][ECAL_skirocID][ECAL_sca] = bcid_cur;
                }
                for(auto it_BIF = BIF_events.begin(); it_BIF != BIF_events.end(); ++it_BIF){
                    if(it_BIF->first - bcid_cur > ECAL_EventMerge)continue;
                    if(fabs( it_BIF->first - bcid_cur) < ECAL_EventMerge){
                        BIF_bcid = it_BIF->first;
                        BIF_TDC = it_BIF->second;
                        Evt_num_matched++;
                        bcid_matched = true;
                        break;
                    }
                }
                if(bcid_matched)tree->Fill();
            }
        }     
        catch (DataNotAvailableException& e) {
            cerr << "Error in reading ECAL and BIF collection: " << e.what() << endl; 
        }
        /*try {
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
            }*/


        
    }
    cout << "Total events processed: " << Evt_num <<" matched events: " << Evt_num_matched << endl;
    
    reader->close();
    rootFile->Write();
    rootFile->Close();
    auto end = std::chrono::high_resolution_clock::now();
    cout << "Data extraction and ROOT file creation completed. in " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds." << endl;
    return 0;
}

void ECAL_Clear(){
    ECAL_ClearVectors();
    for (int i = 0; i < ECAL_LayerNo; i++) {
        for (int j = 0; j < ECAL_ChipNo; j++) {
            ECAL_bcid_loop[i][j] = 0;
            for (int k = 0; k < ECAL_SCANo; k++) {
                ECAL_corrected_bcid[i][j][k] = 0;
                ECAL_badbcid[i][j][k] = 0;
                ECAL_nhits[i][j][k] = 0;
            }
        }
    }
}
void ECAL_ClearVectors() {
    ECAL_layer_ptr->clear();
    ECAL_chip_ptr->clear();
    ECAL_sca_ptr->clear();
    ECAL_channel_ptr->clear();
    ECAL_adc_high_ptr->clear();
    ECAL_adc_low_ptr->clear();
    ECAL_hitbit_high_ptr->clear();
    ECAL_hitbit_low_ptr->clear();
    ECAL_gain_high_ptr->clear();
    ECAL_gain_low_ptr->clear();
}
    

void ECAL_ReadInt(LCGenericObject* objECAL) {
    ECAL_cycleNr = objECAL->getIntVal(0);
    ECAL_bunchXID = objECAL->getIntVal(1);
    ECAL_sca = objECAL->getIntVal(2);
    ECAL_layer = objECAL->getIntVal(3);
    ECAL_skirocID = objECAL->getIntVal(4);
    ECAL_nChannels = objECAL->getIntVal(5);
}
void ECAL_ReadVector(LCGenericObject* objECAL) {
    ECAL_nChannels = objECAL->getIntVal(5);
    for (int j = 0; j < ECAL_nChannels; ++j) {
        int base = 6 + j * 6;  // 每个通道的信息紧挨着，每 6 个数据为一组
        /*if(hitbit_low != hitmap_high){
            cout << "Hit bit low: " << hitbit_low << "!= Hit bit high: " << hitbit_high << endl;
            continue;
        }
        if(hitbit_low == 0){
            continue;
        }*/
        ECAL_layer_ptr->push_back(ECAL_layer);
        ECAL_chip_ptr->push_back(ECAL_skirocID);
        ECAL_sca_ptr->push_back(ECAL_sca);
        ECAL_channel_ptr->push_back(j);
        ECAL_adc_low_ptr->push_back(objECAL->getIntVal(base));
        ECAL_hitbit_low_ptr->push_back(objECAL->getIntVal(base+2));
        ECAL_gain_low_ptr->push_back(objECAL->getIntVal(base + 1));
        ECAL_gain_high_ptr->push_back(objECAL->getIntVal(base + 4));
        ECAL_adc_high_ptr->push_back(objECAL->getIntVal(base + 3));
        ECAL_hitbit_high_ptr->push_back(objECAL->getIntVal(base + 5));
    }
}

std::map<int, std::vector<int>> ECAL_BuildEvents(LCCollection* ecalcollection, int ECAL_EventMerge) {
    std::map<int, std::vector<int>> events;//corrected bcid and i_element
    int corrected_bcid;
    bool b_empty;
    ECAL_Clear();
    for (int i = 0; i < ecalcollection->getNumberOfElements(); ++i) {
        LCGenericObject* objECAL = dynamic_cast<LCGenericObject*>(ecalcollection->getElementAt(i));
        if (!objECAL) continue;
        ECAL_ReadInt(objECAL);
        if(find(ECAL_masked_chips.begin(), ECAL_masked_chips.end(), ECAL_layer*100+ECAL_skirocID) != ECAL_masked_chips.end())continue;
        //Only check the hit bit to save time
        ECAL_nChannels = objECAL->getIntVal(5);
        for (int i_channel = 0; i_channel < ECAL_nChannels; ++i_channel) {
            int base = 6 + i_channel * 6; 
            if(objECAL->getIntVal(base) != 0 || objECAL->getIntVal(base + 3) != 0){
                ECAL_nhits[ECAL_layer][ECAL_skirocID][ECAL_sca]++;
                break;
            }
        }//hit bit checked
        cycle_current = ECAL_cycleNr;
        if(ECAL_sca>0 && (ECAL_bunchXID + ECAL_bcid_loop[ECAL_layer][ECAL_skirocID]*4096 < ECAL_corrected_bcid[ECAL_layer][ECAL_skirocID][ECAL_sca-1]) ){
            ECAL_bcid_loop[ECAL_layer][ECAL_skirocID]++;
        }
        ECAL_corrected_bcid[ECAL_layer][ECAL_skirocID][ECAL_sca] = ECAL_bunchXID + ECAL_bcid_loop[ECAL_layer][ECAL_skirocID]*4096;
        ECAL_entry[ECAL_layer][ECAL_skirocID][ECAL_sca] = i;
    }
    GetBadBCID(ECAL_badbcid, ECAL_corrected_bcid, ECAL_nhits);
    for(int i_layer=0; i_layer<ECAL_LayerNo; i_layer++){
        for(int i_chip=0; i_chip<ECAL_ChipNo; i_chip++){
            for(int i_sca=0; i_sca<ECAL_SCANo; i_sca++){
                if(ECAL_nhits[i_layer][i_chip][i_sca] >0){
                    cout<<"1 ECAL layer "<<i_layer<<" chip "<<i_chip<<" sca "<<i_sca<<" bcid "<<ECAL_corrected_bcid[i_layer][i_chip][i_sca]<<" nhits "<<ECAL_nhits[i_layer][i_chip][i_sca]<<endl;
                }

                if(ECAL_nhits[i_layer][i_chip][i_sca] < 1)continue;
                if(ECAL_badbcid[i_layer][i_chip][i_sca] != 0)continue;
                events[ECAL_corrected_bcid[i_layer][i_chip][i_sca]].push_back(ECAL_entry[i_layer][i_chip][i_sca]);
            }
        }
    }
    events = mergeCloseBCIDs(events, ECAL_EventMerge);
    cout<<"ECAL current cycle "<<cycle_current<<endl;
    cout<<"ECAL bcid";
    for(auto it=events.begin(); it!=events.end(); it++){
        cout<<" "<<it->first;
    }
    cout<<endl;
    return events;
}

std::map<int, std::vector<int>> mergeCloseBCIDs(const std::map<int, vector<int>>& merge_map, int threshold) {
    std::map<int, std::vector<int>> merged;
    auto it = merge_map.begin();

    if (it == merge_map.end()) return merged;

    int current_key = it->first;
    merged[current_key]=(it->second);
    ++it;
    for (; it != merge_map.end(); ++it) {
        int bcid = it->first;
        if (bcid - current_key <= threshold) {
            merged[current_key] = mergeVectorsWithCheck(merged[current_key], it->second);
        } 
        else{
            current_key = bcid;
            merged[current_key]=(it->second);
        }
    }
    return merged;
}

std::vector<int> mergeVectorsWithCheck(const std::vector<int>& v1, const std::vector<int>& v2) {
    std::set<int> elements(v1.begin(), v1.end());
    for (int val : v2) {
        if (elements.count(val)) {
            throw std::runtime_error("Duplicate element found: " + std::to_string(val));
            return {};
        }
        elements.insert(val);
    }
    return std::vector<int>(elements.begin(), elements.end());
}

std::map<int, int> BIF_BuildEvents(LCCollection* BIFcollection) {
    std::map<int, int> events={};
    if(BIFcollection->getNumberOfElements() !=1){
        cout << "BIF collection size != 1 " << BIFcollection->getNumberOfElements() << endl;
        return events;
    }
    LCGenericObject* objBIF = dynamic_cast<LCGenericObject*>(BIFcollection->getElementAt(0));
    if(objBIF->getNInt()<8){
        cout << "BIF Obj length < 8 " << objBIF->getNInt() << endl;
        return events;
    }    
    BIF_acqNumber = (unsigned int)objBIF->getIntVal(0);
    BIF_cycleNr = (unsigned)objBIF->getIntVal(1);
    BIF_Start=(unsigned int)objBIF->getIntVal(2) + (unsigned int)objBIF->getIntVal(3)*0x100000000 + BIF_TDC_Offset;
    for(int i=4; i<objBIF->getNInt(); i+=4){
        BIF_End=(unsigned int)objBIF->getIntVal(i+2) + (unsigned int)objBIF->getIntVal(i+3)*0x100000000;
        BIF_bcid = (BIF_End - BIF_Start)/256;
        BIF_TDC = (BIF_End - BIF_Start)%256;
        events[BIF_bcid] = BIF_TDC;
    }   
    cout<<hex<<"BIF acqNumber "<<BIF_acqNumber<<" BIF cycleNr "<<BIF_cycleNr<<" BIF Start "<<BIF_Start<<dec<<endl;
    cout<<"BIF bcid";
    for(auto it=events.begin(); it!=events.end(); it++){
        cout<<" "<<it->first;
    }
    cout<<endl;
    return events;
}
void GetBadBCID(int _badbcid[ECAL_LayerNo][ECAL_ChipNo][ECAL_SCANo], int _corrected_bcid[ECAL_LayerNo][ECAL_ChipNo][ECAL_SCANo], int _nhits[ECAL_LayerNo][ECAL_ChipNo][ECAL_SCANo]) {
    for(int i=0; i<ECAL_LayerNo; i++) {
      for (int k=0; k<ECAL_ChipNo; k++) {
	    for (int ibc=0; ibc<ECAL_SCANo-2; ibc++) {
	    // if sca+1 is filled with consec bcid, and sca+2 also, then _badbcid[sca]==3 && _badbcid[sca+1]==3 (retriggering)
	    // if sca+1 is not filled with consec bcid,  _badbcid==0
	    // if sca and sca+1 have consecutive bcids but are not retriggers, then we consider 3 types of EMPTY events
	    //     case A: empty events after a event with triggers
	    //     case B: empty events before a event with triggers --> TO BE UNDERSTOOD but it seems that the good one is the one with the trigger (the second)
	    //     case C&D: both bcids have triggers but, we do nothing
	    if(ibc==0) {
	        _badbcid[i][k][ibc]=0;
	        int _corr_bcid=_corrected_bcid[i][k][ibc];
	        int _corr_bcid1=0;
	        int _corr_bcid2=0;

	        if(_corrected_bcid[i][k][ibc+1]>0 && _corrected_bcid[i][k][ibc]>0 && (_corrected_bcid[i][k][ibc+1]-_corrected_bcid[i][k][ibc])>0) _corr_bcid1=_corrected_bcid[i][k][ibc+1];

	        if(_corrected_bcid[i][k][ibc+2]>0 && (_corrected_bcid[i][k][ibc+2]-_corrected_bcid[i][k][ibc+1])>0) _corr_bcid2=_corrected_bcid[i][k][ibc+2];

	        if(_corr_bcid2>0) {
		    //empty events
		        if( ( _corr_bcid2-_corr_bcid1) >(BCIDTHRES - 1) && (_corr_bcid1-_corr_bcid) ==1) {
		        //case A: empty events after a event with triggers
		        if(_nhits[i][k][ibc]>0 && _nhits[i][k][ibc+1]==0) {
		            _badbcid[i][k][ibc]=0;
		            _badbcid[i][k][ibc+1]=2;//this one is to not be used in any case
		        }
		        //case B: empty events before a event with triggers --> TO BE UNDERSTOOD but it seems that the good one is the one with trigger (the second bcid)
		        if(_nhits[i][k][ibc]==0 && _nhits[i][k][ibc+1]>0) {
		            _badbcid[i][k][ibc]=1; //empty event
		            _badbcid[i][k][ibc+1]=0;//good one
		        }
		        //case C&D: both bcids have triggers but, we do nothing
                }
		    // pure retriggers
		        if( ( _corr_bcid2-_corr_bcid1) < BCIDTHRES && (_corr_bcid1-_corr_bcid) < BCIDTHRES) {
		            _badbcid[i][k][ibc]=3;
		            _badbcid[i][k][ibc+1]=3;
		            _badbcid[i][k][ibc+2]=3;
		        }
	        }

	        // pure retriggers
	        if( _corr_bcid1 > 0 && (_corr_bcid1-_corr_bcid) > 1 && (_corr_bcid1-_corr_bcid) <BCIDTHRES) {
		        _badbcid[i][k][ibc]=3;
		        _badbcid[i][k][ibc+1]=3;
	        }  
	    } //ibc==0 if 

	    if(ibc>0 && _badbcid[i][k][ibc]<0 && _corrected_bcid[i][k][ibc] >0 && (_corrected_bcid[i][k][ibc]-_corrected_bcid[i][k][ibc-1])>0 ) {
	        _badbcid[i][k][ibc]=0;
	        int _corr_bcid=_corrected_bcid[i][k][ibc];
	        int _corr_bcidminus=_corrected_bcid[i][k][ibc-1];

	        if(_corrected_bcid[i][k][ibc+1]>0 && (_corrected_bcid[i][k][ibc+1]-_corrected_bcid[i][k][ibc])>0) {
		        int _corr_bcid1=_corrected_bcid[i][k][ibc+1];
		        if(_corrected_bcid[i][k][ibc+2]>0 && (_corrected_bcid[i][k][ibc+2]-_corrected_bcid[i][k][ibc+1])>0) {
		            int _corr_bcid2=_corrected_bcid[i][k][ibc+2];
		            if( ( _corr_bcid2-_corr_bcid1) < BCIDTHRES && (_corr_bcid1-_corr_bcid) < BCIDTHRES) {
		                _badbcid[i][k][ibc]=3;
		                _badbcid[i][k][ibc+1]=3;
		                _badbcid[i][k][ibc+2]=3;
		            }
		            if( (_corr_bcid1-_corr_bcid) < BCIDTHRES && (_corr_bcid-_corr_bcidminus) < BCIDTHRES )  _badbcid[i][k][ibc]=3;

		            if( _badbcid[i][k][ibc]!=3 && ( _corr_bcid2-_corr_bcid1) >(BCIDTHRES - 1) && (_corr_bcid1-_corr_bcid) ==1) {
		            //case A: empty events after a event with triggers
		                if(_nhits[i][k][ibc]>0 && _nhits[i][k][ibc+1]==0) {
		                    _badbcid[i][k][ibc]=0;
		                    _badbcid[i][k][ibc+1]=2;//this one is to not be used in any case
                        }
		                //case B: empty events before a event with triggers --> TO BE UNDERSTOOD but it seems that the good one is the one with the trgger (the second bcid)
		                if(_nhits[i][k][ibc]==0 && _nhits[i][k][ibc+1]>0) {
		                    _badbcid[i][k][ibc]=1; //empty event before the real event
		                    _badbcid[i][k][ibc+1]=0;//good event
                        }
		                //case C&D: both bcids have triggers: we do nothing
                        }
		            if( _badbcid[i][k][ibc]!=3 && ( _corr_bcid2-_corr_bcid1) >(BCIDTHRES - 1) && (_corr_bcid1-_corr_bcid) > 1 && (_corr_bcid1-_corr_bcid) <BCIDTHRES) {
		                _badbcid[i][k][ibc]=3;
		                _badbcid[i][k][ibc+1]=3;
                    }
		            if( (_corr_bcid-_corr_bcidminus) < BCIDTHRES ) _badbcid[i][k][ibc]=3;
                } 
                else{
		            if( (_corr_bcid1-_corr_bcid) < BCIDTHRES ) _badbcid[i][k][ibc]=3;
		            if( (_corr_bcid-_corr_bcidminus) < BCIDTHRES ) _badbcid[i][k][ibc]=3;
		        } //ibc+2 if
	      } 
          else {
		    if( (_corr_bcid-_corr_bcidminus) < BCIDTHRES ) _badbcid[i][k][ibc]=3;
	      }//ibc+1 if
	    } //ibc>0 if 

      
	    //tag zero (under/over flow) data
	    //	count_negdata=0;
	
	    //	for (int ichan=0; ichan<NB_OF_CHANNELS_IN_SKIROC; ichan++) {
	    //  if  (adc_high[i][k][ibc][ichan] < NEGDATA_THR) count_negdata++;
	    //}//ichan

	    //if (count_negdata>0) {_badbcid[i][k][ibc]+=32;}

	  }//ibc
      }//k
    }//i   slboard
  
  }    