#include "ECAL.h"
#include "BIF.h"
#include "cepcplotstyle.h"
#include <chrono>
using namespace std;
int main(int argc, char* argv[]) {
    if(argc != 6) {
        cout <<"Wrong number of arguments! should be 6"<<endl;
        return 0;
    }
    auto start = std::chrono::high_resolution_clock::now();
    std::string Mapfile = argv[1];
    std::string datalist = argv[2]; 
    std::string output = argv[3];
    int BIF_cycleNr_offset = atoi(argv[4]);
    int HCAL_cycleNr_offset = atoi(argv[5]);
    BIF bif(Mapfile,BIF_cycleNr_offset,HCAL_cycleNr_offset);//for scanning the offset
    //BIF bif(Mapfile);//for normal event building
    bif.ScanOffset(datalist, output);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "TDC execution time: " << elapsed.count() << " seconds" << std::endl;
    return 1;
}
BIF::BIF(string Mapfile, int BIF_cycleNr_offset, int HCAL_cycleNr_offset, string Maskfile) : ECAL(Mapfile, Maskfile) {
    _BIF_cycleNr_offset = BIF_cycleNr_offset;
    _HCAL_cycleNr_offset = HCAL_cycleNr_offset;
    _BIF_cycleNr=0;
    _BIF_TDC_v = new vector<int>;
    _BIF_bcid_v = new vector<int>;
    HCAL_bcid_v = new vector<int>;
    BranchMap_int["BIF_cycle"] = &_BIF_cycleNr;
    BranchMap_int_v["BIF_bcid"] = _BIF_bcid_v;
    BranchMap_int_v["BIF_TDC"] = _BIF_TDC_v;
    BranchMap_int["HCAL_cycle"] = &HCAL_cycleNr;
    BranchMap_int_v["HCAL_bcid"] = HCAL_bcid_v; 
}
void BIF::Clear(){
    ECAL::Clear();
    _BIF_cycleNr=0;
    _BIF_TDC_v->clear();
    _BIF_bcid_v->clear();
    HCAL_cycleNr = 0;
    HCAL_bcid_v->clear();
}
int BIF::ReadDatalist(string datalist, vector<string>&runs, vector<string>&rootFiles, vector<string>&rootFiles_BIF){
    ifstream infile(datalist);
    if (!infile) {
        cerr << "Error: Cannot open datalist file: " << datalist << endl;
        return 0;
    }
    string line,run_name,file_name,file_name_BIF;
    while (getline(infile, line)){
        istringstream iss(line);
        if(iss >> run_name >> file_name >> file_name_BIF){
            runs.push_back(run_name);
            rootFiles.push_back(file_name);
            rootFiles_BIF.push_back(file_name_BIF);
        }
        else{cerr << "Error: Failed to parse line: " << line << endl;}
    }
    infile.close();
    cout << "Found " << rootFiles.size() << " ROOT files and "<< rootFiles_BIF.size() <<" BIF files in " <<datalist << endl;
    return 1;
}
int BIF::Match_bcid(int *Matched_Events,int Scanning_No, std::map<int, std::vector<int>> &BIF_cycle_bcid_map, std::map<int, std::vector<int>> &ECAL_cycle_bcid_map){
    if(BIF_cycle_bcid_map.size() != ECAL_cycle_bcid_map.size()){
        cerr << "Error: The number of cycles in BIF and ECAL do not match" << endl;
        return 0;
    }
    auto it_BIF = BIF_cycle_bcid_map.begin();
    auto it_ECAL = ECAL_cycle_bcid_map.begin();
    while(it_BIF != BIF_cycle_bcid_map.end() && it_ECAL != ECAL_cycle_bcid_map.end()){
        if(it_BIF->first != it_ECAL->first){
            cerr << "Error: Cycle numbers do not match: BIF cycle " << it_BIF->first << " vs ECAL cycle " << it_ECAL->first << endl;
            continue;
        }
        for(size_t i_ECAL=0; i_ECAL < it_ECAL->second.size(); i_ECAL++){
            int bcid_ECAL = it_ECAL->second[i_ECAL];
            for(size_t i_BIF=0; i_BIF < it_BIF->second.size(); i_BIF++){
                int bcid_BIF = it_BIF->second[i_BIF];
                if(fabs(bcid_ECAL - bcid_BIF) <= Scanning_No/2){
                    Matched_Events[bcid_ECAL - bcid_BIF + Scanning_No/2]++;
                }
            }  
        }
        it_BIF++;
        it_ECAL++;
    }
    return 1;
}
int BIF::Scan_BCIDOffset(int *Matched_Events, int Scanning_No,int offset ,TTree *tree_BIF, TTree *tree_ECAL,string str_Type="BIF"){
    cout<< "Scanning BCID offset for " << str_Type << " with offset " << offset << endl;
    int entry_BIF = 0;
    int entry_ECAL = 0;
    int *cycle;
    curr_cycle = -1;
    std::map<int, std::vector<int>> cycle_bcid_map;
    std::map<int, std::vector<int>> ECAL_cycle_bcid_map;
    while(entry_BIF<tree_BIF->GetEntries() &&entry_ECAL<tree_ECAL->GetEntries() && entry_ECAL<Read_EventNum){
        if(entry_ECAL%1000==0)cout<<"Entry "<<entry_ECAL<<" in "<<tree_ECAL->GetEntries()<<" for ECAL, Entry "<<entry_BIF<<" in "<<tree_BIF->GetEntries()<<" for BIF"<<endl;
        Clear();
        tree_ECAL->GetEntry(entry_ECAL);
        tree_BIF->GetEntry(entry_BIF);
        //Match cycle ID
        if(str_Type == "BIF"){
            cycle = &_BIF_cycleNr;
        }else if(str_Type == "HCAL"){
            cycle = &HCAL_cycleNr;
        }else{
            cerr << "Error: Unknown str_Type " << str_Type << endl;
            return 0;
        }
                
        while((*cycle + offset)< _acqNumber && entry_BIF<tree_BIF->GetEntries()){
            entry_BIF++;
            tree_BIF->GetEntry(entry_BIF);
        }
        while((*cycle + offset)> _acqNumber && entry_ECAL<tree_ECAL->GetEntries() && entry_ECAL<Read_EventNum){
            entry_ECAL++;
            tree_ECAL->GetEntry(entry_ECAL);
        }
        if((*cycle + offset)!=_acqNumber){continue;}    
        if(_acqNumber > curr_cycle){
            curr_cycle = _acqNumber;
            Match_bcid(Matched_Events, Scanning_No,cycle_bcid_map, ECAL_cycle_bcid_map);    
            cycle_bcid_map.clear();
            ECAL_cycle_bcid_map.clear();
        }  
        //BIF ECAL match Over  
        auto it_cycle = cycle_bcid_map.find(curr_cycle);
        if(it_cycle == cycle_bcid_map.end()){
            if(str_Type == "BIF") cycle_bcid_map[curr_cycle] = *_BIF_bcid_v;
            else if(str_Type == "HCAL") cycle_bcid_map[curr_cycle] =*HCAL_bcid_v;
            else{
                cerr << "Error: Unknown str_Type " << str_Type << endl;
                return 0;
            }
        }            
        ECAL_curr_bcid = -1;
        for(int i_hit=0; i_hit<_hitbit_high_ptr->size(); i_hit++){
            hitbit_high=_hitbit_high_ptr->at(i_hit);
            hitbit_low=_hitbit_low_ptr->at(i_hit);
            if(_hitbit_high_ptr->at(i_hit)==0 || _hitbit_low_ptr->at(i_hit)==0)continue;
            layer=_layer_ptr->at(i_hit);
            chip=_chip_ptr->at(i_hit);
            sca=_sca_ptr->at(i_hit);
            channel=_channel_ptr->at(i_hit);
            if(ECAL_curr_bcid == -1){
                ECAL_curr_bcid = _bcid[layer][chip][sca];
            }
            else if(fabs(ECAL_curr_bcid - _bcid[layer][chip][sca]) > 3){
                cerr << "Error: Multiple BCIDs in one event for layer" << layer << ", chip " << chip << ", sca " << sca <<", channel " << channel << endl;
                continue;
            }
        }
        ECAL_cycle_bcid_map[_acqNumber].push_back(ECAL_curr_bcid);
        entry_ECAL++;
    }
    return 1;
}
int BIF::ScanOffset(string datalist, string output){
    //initialize datalist & histograms
    #pragma region
    vector<string> runs;
    vector<string> rootFiles;
    vector<string> rootFiles_BIF;
    if(ReadDatalist(datalist,runs,rootFiles,rootFiles_BIF)!=1){
        cerr << "Error: Unable to read datalist" << endl;
        return 0;
    }
    if(rootFiles.size() != rootFiles_BIF.size()){
        cerr << "Error: The number of ROOT files and BIF files do not match" << endl;
        return 0;
    }
    //init_histograms
    for(int i_layer=selected_layer; i_layer<selected_layer+1; i_layer++){
        string_buffer = Form("hitmap_ChipChannel_layer%d",i_layer);
        h_hitmap[i_layer][0] = new TH2F(string_buffer.c_str(), string_buffer.c_str(), Chip_No, -0.5, Chip_No-0.5, Channel_No, -0.5, Channel_No-0.5);
        FormatData(h_hitmap[i_layer][0], "Chip", "Channel", "Number of hits", 1.1, 1.1, 1.1);
        string_buffer = Form("hitmap_XY_layer%d",i_layer);
        h_hitmap[i_layer][1] = new TH2F(string_buffer.c_str(), string_buffer.c_str(), Cell_No, RangeX[0]-CellSize/2., RangeX[1]+CellSize/2., Cell_No, RangeY[0]-CellSize/2., RangeY[1]+CellSize/2);
        FormatData(h_hitmap[i_layer][1], "X [mm]", "Y [mm]", "Number of hits", 1.1, 1.1, 1.1);
        for(int i_chip=0; i_chip<Chip_No; i_chip++){
            for(int i_channel=0; i_channel<Channel_No; i_channel++){
                string_buffer = Form("h_TDC_layer%d_chip%d_channel%d",i_layer,i_chip,i_channel);
                h_TDC[i_layer][i_chip][i_channel] = new TH1F(string_buffer.c_str(), string_buffer.c_str(), 4096, 0, 4096);
                FormatData(h_TDC[i_layer][i_chip][i_channel], "Output [TDC]", "Number of hits", 1.1, 1.1);
            }
        }
    }
    for(int i=0; i<Scanning_No; i++){
        Matched_Events_BIF[i] = 0;
        Matched_Events_HCAL[i] = 0;
    }
    #pragma endregion

    //Loop over ROOT files
    #pragma region
    for(int i=0; i< rootFiles.size(); i++){
        string rootFile = rootFiles[i];
        string rootFile_BIF = rootFiles_BIF[i];
        cout<<"Read "<<rootFile<<" and BIF file "<<rootFile_BIF<<endl;        
        TFile *fin_ECAL = TFile::Open(rootFile.c_str(), "READ");
        TFile *fin_BIF = TFile::Open(rootFile_BIF.c_str(), "READ");
        if (!fin_ECAL || fin_ECAL->IsZombie()) {
            cerr << "Error: Cannot open ROOT file: " << rootFile << endl;
            continue;
        }
        if (!fin_BIF || fin_BIF->IsZombie()) {
            cerr << "Error: Cannot open BIF file: " << rootFile_BIF << endl;
            fin_BIF->Close();
            continue;
        }
        TTree *tree_ECAL = (TTree*)fin_ECAL->Get(treename_selected.c_str());
        if (!tree_ECAL) {
            cerr << "Error: Cannot open TTree "<<treename_selected <<" in " << rootFile << endl;
            fin_ECAL->Close();
            continue;
        }
        TTree *tree_BIF = (TTree*)fin_BIF->Get(treename_BIF.c_str());
        if (!tree_BIF) {
            cerr << "Error: Cannot open TTree "<<treename_BIF <<" in " << rootFile_BIF << endl;
            fin_BIF->Close();
            continue;
        }
        ReadTree(tree_ECAL,BranchNames_selected,BranchDesciptions_selected);
        ReadTree(tree_BIF,BranchNames_BIF,BranchDesciptions_BIF);
        Scan_BCIDOffset(Matched_Events_BIF, Scanning_No, _BIF_cycleNr_offset, tree_BIF, tree_ECAL, "BIF");
        Scan_BCIDOffset(Matched_Events_HCAL, Scanning_No, _HCAL_cycleNr_offset, tree_BIF, tree_ECAL, "HCAL");
        fin_ECAL->Close();
        cout<<"Read over "<<rootFile<<endl;
    }
    #pragma endregion
    //Analyze cycle bcid map
    
    //Write histograms to output file
    #pragma region
    TFile *fout = new TFile(output.c_str(), "RECREATE");
    if (!fout || fout->IsZombie()) {
        cerr << "Error: Cannot open output file: " << output << endl;
        return 0;
    }
    rootlogon();
    g_BIF_Offset = new TGraph();
    g_BIF_Offset->SetName("g_BIF_Offset");
    g_BIF_Offset->SetTitle("BIF Offset");
    FormatData(g_BIF_Offset, "BCID Offset", "Number of Matched Events", 1.1, 1.1);
    for(int i=0; i<Scanning_No; i++){
        g_BIF_Offset->SetPoint(i, i - Scanning_No/2, Matched_Events_BIF[i]);
    }
    TCanvas *c_BIF_Offset = new TCanvas("c_BIF_Offset", "BIF Offset", 800, 600);
    g_BIF_Offset->Draw("ALP");
    g_BIF_Offset->Write();
    c_BIF_Offset->Write();
    g_HCAL_Offset = new TGraph();
    g_HCAL_Offset->SetName("g_HCAL_Offset");
    g_HCAL_Offset->SetTitle("HCAL Offset");
    FormatData(g_HCAL_Offset, "BCID Offset", "Number of Matched Events", 1.1, 1.1);
    for(int i=0; i<Scanning_No; i++){
        g_HCAL_Offset->SetPoint(i, i - Scanning_No/2, Matched_Events_HCAL[i]);
    }
    TCanvas *c_HCAL_Offset = new TCanvas("c_HCAL_Offset", "HCAL Offset", 800, 600);
    g_HCAL_Offset->Draw("ALP");
    g_HCAL_Offset->Write();
    c_HCAL_Offset->Write();
    for(int i_layer=selected_layer; i_layer<selected_layer+1; i_layer++){
        for(int i_chip=0; i_chip<Chip_No; i_chip++){
            for(int i_channel=0; i_channel<Channel_No; i_channel++){
                string_buffer = Form("Layer%d/Chip%d/Channel%d",i_layer,i_chip,i_channel);
                fout->cd();
                fout->mkdir(string_buffer.c_str());
                fout->cd(string_buffer.c_str());
                h_TDC[i_layer][i_chip][i_channel]->Write();
            }
        }
        fout->cd(Form("Layer%d",i_layer));
        h_hitmap[i_layer][0]->Write();
        h_hitmap[i_layer][1]->Write();
    }                
    fout->Close();
    #pragma endregion
    
    return 1;
}