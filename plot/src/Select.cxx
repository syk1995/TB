#include "ECAL.h"
#include "Select.h"
#include "cepcplotstyle.h"
#include <chrono>
using namespace std;
int main(int argc, char* argv[]) {
    if(argc != 4) {
        cout <<"Wrong number of arguments! should be 3"<<endl;
        return 0;
    }
    auto start = std::chrono::high_resolution_clock::now();
    std::string Mapfile = argv[1];
    std::string datalist = argv[2]; 
    std::string output = argv[3];
    ECAL ecal(Mapfile);
    ecal.HitMap(datalist, output);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "HitMap execution time: " << elapsed.count() << " seconds" << std::endl;
    return 1;
}
int ECAL::HitMap(string datalist, string output){
    //initialize datalist & histograms
    #pragma region
    vector<string> runs;
    vector<string> rootFiles;
    if(ReadDatalist(datalist,runs,rootFiles)!=1){
        cerr << "Error: Unable to read datalist" << endl;
        return 0;
    }
    //init_histograms
    h_hitbit_high = new TH1F("h_hitbit_high","Hitbit High", 2200, -1100, 100);
    FormatData(h_hitbit_high,"Hitbit high","Number of hits",1.1,1.1);
        
    h_hitbit_low = new TH1F("h_hitbit_low","Hitbit Low", 3000, -2000, 1000);
    FormatData(h_hitbit_low,"Hitbit low","Number of hits",1.1,1.1);

    int bcid_bin[3]={6500, -1500, 5000};
    h_bcid = new TH1F("h_bcid","BCID", bcid_bin[0], bcid_bin[1], bcid_bin[2]);
    FormatData(h_bcid,"BCID","Number of chipss",1.1,1.1);

    h_corrected_bcid = new TH1F("h_corrected_bcid","Corrected BCID", bcid_bin[0], bcid_bin[1], bcid_bin[2]);
    FormatData(h_corrected_bcid,"Corrected BCID","Number of chips",1.1,1.1);

    h_badbcid = new TH1F("h_badbcid","Bad BCID", bcid_bin[0], bcid_bin[1], bcid_bin[2]);
    FormatData(h_badbcid,"Bad BCID","Number of chips",1.1,1.1);

    TFile *fout = new TFile(output.c_str(), "RECREATE");
    if (!fout || fout->IsZombie()) {
        cerr << "Error: Cannot open output file: " << output << endl;
        return 0;
    }
    TTree *tree_out = new TTree(treename_selected.c_str(),("Select from "+treename_raw).c_str());
    WriteTree(tree_out,BranchNames_selected,BranchDesciptions_selected);
    WriteTree(tree_out,BranchNames_hitmap,BranchDesciptions_hitmap);
    #pragma endregion

    //Loop over ROOT files
    #pragma region
    for (const auto& rootFile : rootFiles) {
        cout<<"Read "<<rootFile<<endl;
        TFile *fin = TFile::Open(rootFile.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            cerr << "Error: Cannot open ROOT file: " << rootFile << endl;
            continue;
        }
        TTree *tree_in = (TTree*)fin->Get(treename_raw.c_str());
        if (!tree_in) {
            cerr << "Error: Cannot find TTree in " << rootFile << endl;
            fin->Close();
            continue;
        }
        ReadTree(tree_in,BranchNames_raw,BranchDesciptions_raw);
        for(int i_entry = 0; i_entry < tree_in->GetEntries() && i_entry>-1; i_entry++){
            if(i_entry%1000==0)cout<<"Entry "<<i_entry<<" in "<<tree_in->GetEntries()<<endl;
            Clear();
            tree_in->GetEntry(i_entry);
            //Fill histograms
            for(int i_layer = 0; i_layer < Layer_No; i_layer++){
                for(int i_chip = 0; i_chip < Chip_No; i_chip++){
                    for(int i_sca = 0; i_sca < SCA_No; i_sca++){
                        h_badbcid->Fill(_badbcid[i_layer][i_chip][i_sca]);
                        if(_badbcid[i_layer][i_chip][i_sca] !=0) continue;
                        h_bcid->Fill(_bcid[i_layer][i_chip][i_sca]);
                        h_corrected_bcid->Fill(_corrected_bcid[i_layer][i_chip][i_sca]);                        
                        for(int i_channel =0; i_channel < Channel_No; i_channel++){
                            h_hitbit_high->Fill(_hitbit_high[i_layer][i_chip][i_sca][i_channel]);
                            h_hitbit_low->Fill(_hitbit_low[i_layer][i_chip][i_sca][i_channel]);
                            if(_hitbit_high[i_layer][i_chip][i_sca][i_channel]<-10) continue;
                            //Push back to vector
                            #pragma region
                            _layer_ptr->push_back(i_layer);
                            _chip_ptr->push_back(i_chip);
                            _sca_ptr->push_back(i_sca);
                            _channel_ptr->push_back(i_channel);
                            _adc_low_ptr->push_back(_adc_low[i_layer][i_chip][i_sca][i_channel]);
                            _adc_high_ptr->push_back(_adc_high[i_layer][i_chip][i_sca][i_channel]);
                            _autogainbit_low_ptr->push_back(_autogainbit_low[i_layer][i_chip][i_sca][i_channel]);
                            _autogainbit_high_ptr->push_back(_autogainbit_high[i_layer][i_chip][i_sca][i_channel]);
                            _hitbit_low_ptr->push_back(_hitbit_low[i_layer][i_chip][i_sca][i_channel]);
                            _hitbit_high_ptr->push_back(_hitbit_high[i_layer][i_chip][i_sca][i_channel]);
                            #pragma endregion
                        }
                    }
                }
            }
            tree_out->Fill();
        }
        fin->Close();
        cout<<"Read over "<<rootFile<<endl;
    }
    #pragma endregion

    //Write histograms to output file
    #pragma region
    fout->cd();
    h_bcid->Write();
    h_corrected_bcid->Write();
    h_badbcid->Write();
    h_hitbit_high->Write();
    h_hitbit_low->Write();
    tree_out->Write();
    fout->Close();
    #pragma endregion
    
    return 1;
}