
#include "BIF.h"
#include <chrono>
using namespace std;
using namespace lcio;

int BIF(string inputFile, string outputFile) {
    auto start = std::chrono::high_resolution_clock::now();
    //Initialize output root and tree
    #pragma region
    TFile* rootFile = new TFile(outputFile.c_str(), "RECREATE");
    if (!rootFile->IsOpen()) {
        cerr << "Error: Unable to open output ROOT file "<<outputFile << endl;
        return -1;
    }
    BIF_bcid_v = new std::vector<int>;
    BIF_TDC_v = new std::vector<int>;
    TTree* tree = new TTree("BIF", "BIF");
    tree->Branch("BIF_cycle", &BIF_cycleNr, "BIF_cycle/I");
    tree->Branch("BIF_bcid", BIF_bcid_v);
    tree->Branch("BIF_TDC", BIF_TDC_v);
    tree->Branch("HCAL_cycle", &HCAL_cycleNr, "HCAL_cycle/I");
    tree->Branch("HCAL_bcid", &HCAL_events);
    #pragma endregion

    LCReader* reader = LCFactory::getInstance()->createLCReader();
    LCEvent* event = nullptr;
    LCCollection* BIFcollection = nullptr;
    LCGenericObject* objBIF = nullptr;
    
    cout<<"Read "<<inputFile<<endl;
    try {
        reader->open(inputFile);
        std::cout << "File opened successfully." << std::endl;
    } catch (lcio::IOException& e) {
        std::cerr << "Error opening file: " << e.what() << std::endl;
        return -1;
    }
    Evt_num=0;
    for(int i = 0; (event = reader->readNextEvent()) && i>-1; i++){
        if(i%1000==0)cout << "Processing event " << i << endl;
        BIF_Clear();
        HCAL_Clear();
        try{
            LCCollection* hcalcollection = event->getCollection("EUDAQDataScCAL");
            HCAL_events = HCAL_BuildEvents(hcalcollection);
            BIFcollection = event->getCollection("EUDAQDataBIF");
            BIF_events = BIF_BuildEvents(BIFcollection);
            for(auto it_BIF = BIF_events.begin(); it_BIF != BIF_events.end(); ++it_BIF){
                BIF_bcid_v->push_back(it_BIF->first);
                BIF_TDC_v->push_back(it_BIF->second);
            }
            Evt_num++;
            tree->Fill();
            /*cout << "Event number: " << Evt_num << " acqNumber: " << BIF_acqNumber << " cycleNr: " << BIF_cycleNr << endl;
            for(int j=0; j<BIF_bcid_v->size(); j++){
                cout << "BIF_bcid: " << BIF_bcid_v->at(j) << " BIF_TDC: " << BIF_TDC_v->at(j) << endl;
            }*/
        }
        catch (DataNotAvailableException& e) {
            cerr << "Collection 'EUDAQDataBIF' not found in this event." << endl;
        }
    }            
    cout << "Total events processed: " << Evt_num << endl;
    
    reader->close();
    rootFile->Write();
    rootFile->Close();
    auto end = std::chrono::high_resolution_clock::now();
    cout << "Data extraction and ROOT file creation completed. in " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds." << endl;
    return 0;
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
        BIF_TDC = (BIF_End - BIF_Start)%256;
        BIF_bcid = (BIF_End - BIF_Start - BIF_TDC)/256;
        events[BIF_bcid] = BIF_TDC;
    }   
    return events;
}
std::vector<int> HCAL_BuildEvents(LCCollection* hcalcollection) {
    vector<int> HCAL_bcid_v;
    for (int i = 0; i < hcalcollection->getNumberOfElements(); ++i) {
        LCGenericObject* obj = dynamic_cast<LCGenericObject*>(hcalcollection->getElementAt(i));
        if (!obj) continue;  
        HCAL_cycleNr = obj->getIntVal(0);
        HCAL_bunchXID = obj->getIntVal(1);
        HCAL_eventNr = obj->getIntVal(2);
        HCAL_chipID = obj->getIntVal(3);
        HCAL_nChannels = obj->getIntVal(4);    
        /*for (int j = 0; j < HCAL_nChannels; ++j) {
            HCAL_tdc14bit.push_back(obj->getIntVal(5 + j));  
            HCAL_adc14bit.push_back(obj->getIntVal(5 + HCAL_nChannels + j));  
        }*/
    }
    return HCAL_bcid_v;
}