#include "ECAL.h"
#include "Select.h"
#include "cepcplotstyle.h"
#include <chrono>
using namespace std;
int main(int argc, char* argv[]) {
    if(argc != 5) {
        cout <<"Wrong number of arguments! should be 4"<<endl;
        return 0;
    }
    auto start = std::chrono::high_resolution_clock::now();
    std::string Mapfile = argv[1];
    std::string MaskFile = argv[2];
    std::string datalist = argv[3]; 
    std::string output = argv[4];
    ECAL ecal(Mapfile, MaskFile);
    ecal.Select(datalist, output);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "HitMap execution time: " << elapsed.count() << " seconds" << std::endl;
    return 1;
}
int ECAL::Select(string datalist, string output){
    //initialize
    #pragma region
    vector<string> runs;
    vector<string> rootFiles;
    if(ReadDatalist(datalist,runs,rootFiles)!=1){
        cerr << "Error: Unable to read datalist" << endl;
        return 0;
    }        
    //init_histograms
    rootlogon();
    h_hitbit_high = new TH1F("h_hitbit_high","Hitbit High", 2200, -1100,100);
    FormatData(h_hitbit_high,"Hitbit High","Number of hits",1.1,1.1);        
    h_hitbit_low = new TH1F("h_hitbit_low","Hitbit Low", 3000, -2000, 1000);
    FormatData(h_hitbit_low,"Hitbit Low","Number of hits",1.1,1.1);
    int bcid_bin[3]={6500, -1500, 5000};
    h_bcid = new TH1F("h_bcid","BCID", bcid_bin[0], bcid_bin[1], bcid_bin[2]);
    FormatData(h_bcid,"BCID","Number of entries",1.1,1.1);
    h_corrected_bcid = new TH1F("h_corrected_bcid","Corrected BCID", bcid_bin[0], bcid_bin[1], bcid_bin[2]);
    FormatData(h_corrected_bcid,"Corrected BCID","Number of entries",1.1,1.1);
    h_badbcid = new TH1F("h_badbcid","Bad BCID", bcid_bin[0], bcid_bin[1], bcid_bin[2]);
    FormatData(h_badbcid,"Bad BCID","Number of entries",1.1,1.1);
    h_bcid0 = new TH1F("h_bcid0","0 for empty,1 for non empty", 2, -0.5, 1.5);
    FormatData(h_bcid0,"Empty or not","Number of entries",1.1,1.1);
    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        hist_name = Form("h_emptyEve_adc_low_layer%d",i_layer);
        h_emptyEve_adc[i_layer][0] = new TH1F(hist_name.c_str(),hist_name.c_str(),5000,0,5000);
        FormatData(h_emptyEve_adc[i_layer][0],"Low gain output[ADC]","Number of hits",1.1,1.1);

        hist_name = Form("h_emptyEve_adc_high_layer%d",i_layer);
        h_emptyEve_adc[i_layer][1] = new TH1F(hist_name.c_str(),hist_name.c_str(),5000,0,5000);
        FormatData(h_emptyEve_adc[i_layer][1],"High gain output[ADC]","Number of hits",1.1,1.1);

        hist_name = Form("h_emptyEve_Map_layer%d",i_layer);
        h_emptyEve_Map[i_layer][0] = new TH2F(hist_name.c_str(),hist_name.c_str(),Chip_No,0,Chip_No,Channel_No,0,Channel_No);
        FormatData(h_emptyEve_Map[i_layer][0],"Chip","Channel","Number of hits",1.1,1.1,1.1);

        hist_name = Form("h_emptyEve_Map_XY_layer%d",i_layer);
        h_emptyEve_Map[i_layer][1] = new TH2F(hist_name.c_str(),hist_name.c_str(),Cell_No,RangeX[0]-CellSize/2.,RangeX[1]+CellSize/2.,Cell_No,RangeY[0]-CellSize/2.,RangeY[1]+CellSize/2);
        FormatData(h_emptyEve_Map[i_layer][1],"X[mm]","Y[mm]","Number of empty entries",1.1,1.1,1.1);

        hist_name = Form("h_emptyEve_bcid_sca_layer%d",i_layer);
        h_emptyEve_bcid_sca[i_layer] = new TH2F(hist_name.c_str(),hist_name.c_str(),BCID_No*2, 0, BCID_No*2, SCA_No, 0, SCA_No);
        FormatData(h_emptyEve_bcid_sca[i_layer],"BCID","SCA","Number of empty entries",1.1,1.1,1.1);
        h_emptyEve_bcid_sca[i_layer]->SetMinimum(1);

        hist_name = Form("h_emptyEve_chip_adc_high_layer%d",i_layer);
        h_emptyEve_chip_adc_high[i_layer] = new TH2F(hist_name.c_str(),hist_name.c_str(),Chip_No*Channel_No,0,Chip_No*Channel_No,5000,0,5000);
        FormatData(h_emptyEve_chip_adc_high[i_layer],"Chip*64+Channel","High gain output[TDC]","Number of hits",1.1,1.1,1.1);
        h_emptyEve_chip_adc_high[i_layer]->SetMinimum(1);

        hist_name = Form("h_emptyEve_chip_bcid_layer%d",i_layer);
        h_emptyEve_chip_bcid[i_layer] = new TH2F(hist_name.c_str(),hist_name.c_str(),Chip_No*Channel_No,0,Chip_No*Channel_No,BCID_No*2, 0, BCID_No*2);
        FormatData(h_emptyEve_chip_bcid[i_layer],"Chip*64+Channel","BCID","Number of hits",1.1,1.1,1.1);

        hist_name = Form("h_emptyEve_bcid_adc_high_layer%d",i_layer);
        h_emptyEve_bcid_adc_high[i_layer] = new TH2F(hist_name.c_str(),hist_name.c_str(),BCID_No*2, 0, BCID_No*2, 5000,0,5000);
        FormatData(h_emptyEve_bcid_adc_high[i_layer],"BCID","High gain output[TDC]","Number of hits",1.1,1.1,1.1);
        h_emptyEve_bcid_adc_high[i_layer]->SetMinimum(1);        
    }

    TFile *fout = new TFile(output.c_str(), "RECREATE");
    if (!fout || fout->IsZombie()) {
        cerr << "Error: Cannot open output file: " << output << endl;
        return 0;
    }
    TTree *tree_out = new TTree(treename_selected.c_str(),("Select from "+treename_raw).c_str());
    WriteTree(tree_out,BranchNames_selected,BranchDesciptions_selected);
    WriteTree(tree_out,BranchNames_hitmap,BranchDesciptions_hitmap);//BranchNames_selected includes the names in Branchnames_hitmap, so this line not needed but keep it for now
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
        for(int i_entry = 0; i_entry < tree_in->GetEntries() && i_entry<10; i_entry++){
            if(i_entry%1000==0)cout<<"Entry "<<i_entry<<" in "<<tree_in->GetEntries()<<endl;
            //Clear variables
            #pragma region
            Clear();
            Select_Clear();
            #pragma endregion
            tree_in->GetEntry(i_entry);
            int bcid_min = 9999;
            for(int i_layer = 0; i_layer < Layer_No; i_layer++){
                for(int i_chip = 0; i_chip < Chip_No; i_chip++){
                    for(int i_sca = 0; i_sca < SCA_No; i_sca++){
                        if(_badbcid[i_layer][i_chip][i_sca] != 0) continue;
                        cout<<"acqNumber "<<_acqNumber<<" bcid "<<_bcid[i_layer][i_chip][i_sca]<<" corrected_bcid "<<_corrected_bcid[i_layer][i_chip][i_sca]<<" layer "<<i_layer<<" chip "<<i_chip<<" sca "<<i_sca<<endl;
                        if(i_sca>0){
                            if(_bcid[i_layer][i_chip][i_sca] < _bcid[i_layer][i_chip][i_sca-1]){
                                bcid_loop[i_layer][i_chip]++;
                            }    
                        }
                        _corrected_bcid[i_layer][i_chip][i_sca] = _bcid[i_layer][i_chip][i_sca] + bcid_loop[i_layer][i_chip]*4096;
                        b_empty = true;                        
                        for(int i_channel = 0; i_channel < Channel_No; i_channel++){
                            if(find(maskchannel.begin(), maskchannel.end(), i_layer*10000+i_chip*100+i_sca) != maskchannel.end())continue;
                            if(_hitbit_high[i_layer][i_chip][i_sca][i_channel] > 0) b_empty = false;
                        }
                        if(_bcid[i_layer][i_chip][i_sca] == 0){
                            h_bcid0->Fill(b_empty?0:1);
                        }   
                        if(b_empty){
                            for(int i_channel = 0; i_channel < Channel_No; i_channel++){
                                h_emptyEve_Map[i_layer][0]->Fill(i_chip,i_channel);
                                h_emptyEve_Map[i_layer][1]->Fill(_Pos[i_layer][i_chip][i_channel][0],_Pos[i_layer][i_chip][i_channel][1]);
                                if(find(maskchannel.begin(), maskchannel.end(), i_layer*10000+i_chip*100+i_sca) != maskchannel.end())continue;
                                h_emptyEve_adc[i_layer][0]->Fill(_adc_low[i_layer][i_chip][i_sca][i_channel]);
                                h_emptyEve_adc[i_layer][1]->Fill(_adc_high[i_layer][i_chip][i_sca][i_channel]);
                                h_emptyEve_chip_adc_high[i_layer]->Fill(i_chip*Channel_No+i_channel, _adc_high[i_layer][i_chip][i_sca][i_channel]);
                                h_emptyEve_chip_bcid[i_layer]->Fill(i_chip*Channel_No+i_channel, _corrected_bcid[i_layer][i_chip][i_sca]);
                                h_emptyEve_bcid_adc_high[i_layer]->Fill(_corrected_bcid[i_layer][i_chip][i_sca], _adc_high[i_layer][i_chip][i_sca][i_channel]);
                            }
                            h_emptyEve_bcid_sca[i_layer]->Fill(_corrected_bcid[i_layer][i_chip][i_sca], i_sca);
                            continue;
                        }     
                        if(_corrected_bcid[i_layer][i_chip][i_sca] <bcid_min){
                            bcid_min = _corrected_bcid[i_layer][i_chip][i_sca];
                        }    
                        bcid_map[_corrected_bcid[i_layer][i_chip][i_sca]].push_back(i_layer*10000+i_chip*100+i_sca);
                    }
                }
            }
            //if(bcid_min !=0) cout<<"abnormal bcid entry "<<i_entry<<" bcid_min "<<bcid_min<<endl;
            merged_bcid_map = mergeCloseBCIDs(bcid_map, merge_delta);
            for (const auto& pair : merged_bcid_map) {
                int bcid = pair.first;
                std::vector<int> encoded_values = pair.second;
                Clear_Vector();
                for (const auto& encoded : encoded_values) {
                    int i_layer = encoded / 10000;
                    int i_chip = (encoded % 10000) / 100;
                    int i_sca = encoded % 100;
                    if(find(maskchannel.begin(), maskchannel.end(), i_layer*10000+i_chip*100+i_sca) != maskchannel.end())continue;
                    h_bcid->Fill(_bcid[i_layer][i_chip][i_sca]);
                    h_corrected_bcid->Fill(_corrected_bcid[i_layer][i_chip][i_sca]);
                    h_badbcid->Fill(_badbcid[i_layer][i_chip][i_sca]);
                    for(int i_channel =0; i_channel < Channel_No; i_channel++){
                        h_hitbit_high->Fill(_hitbit_high[i_layer][i_chip][i_sca][i_channel]);
                        h_hitbit_low->Fill(_hitbit_low[i_layer][i_chip][i_sca][i_channel]);
                        //if(_hitbit_high[i_layer][i_chip][i_sca][i_channel]<0)continue;
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
                tree_out->Fill();
            }   
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
    h_bcid0->Write();
    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        fout->cd();
        string dir_name = Form("Layer%d",i_layer);
        fout->mkdir(dir_name.c_str());
        fout->cd(dir_name.c_str());
        string canvas_name = Form("h_emptyEve_adc_low_layer%d",i_layer);
        TCanvas *c_emptyEve_adc_high = new TCanvas(canvas_name.c_str(),canvas_name.c_str(), 800, 600);
        h_emptyEve_adc[i_layer][0]->Draw();
        c_emptyEve_adc_high->Write();
        delete c_emptyEve_adc_high;

        canvas_name = Form("h_emptyEve_adc_high_layer%d",i_layer);
        TCanvas *c_emptyEve_adc_low = new TCanvas(canvas_name.c_str(),canvas_name.c_str(), 800, 600);
        h_emptyEve_adc[i_layer][1]->Draw();
        c_emptyEve_adc_low->Write();
        delete c_emptyEve_adc_low;

        hist_name = Form("h_emptyEve_Map_layer%d",i_layer);
        TCanvas *c_emptyEve_Map = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h_emptyEve_Map[i_layer][0]->Draw("COLZ");
        c_emptyEve_Map->Write();
        delete c_emptyEve_Map;

        hist_name = Form("h_emptyEve_Map_XY_layer%d",i_layer);
        TCanvas *c_emptyEve_Map_XY = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h_emptyEve_Map[i_layer][1]->Draw("COLZ");
        c_emptyEve_Map_XY->Write();
        delete c_emptyEve_Map_XY;

        hist_name = Form("h_emptyEve_bcid_sca_layer%d",i_layer);
        TCanvas *c_emptyEve_bcid_sca = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h_emptyEve_bcid_sca[i_layer]->Draw("COLZ");
        c_emptyEve_bcid_sca->Write();
        delete c_emptyEve_bcid_sca;

        hist_name = Form("h_emptyEve_chip_adc_high_layer%d",i_layer);
        TCanvas *c_emptyEve_chip_adc_high = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h_emptyEve_chip_adc_high[i_layer]->Draw("COLZ");
        c_emptyEve_chip_adc_high->Write();
        delete c_emptyEve_chip_adc_high;

        hist_name = Form("h_emptyEve_chip_bcid_layer%d",i_layer);
        TCanvas *c_emptyEve_chip_bcid = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h_emptyEve_chip_bcid[i_layer]->Draw("COLZ");
        c_emptyEve_chip_bcid->Write();
        delete c_emptyEve_chip_bcid;

        hist_name = Form("h_emptyEve_bcid_adc_high_layer%d",i_layer);
        TCanvas *c_emptyEve_bcid_adc_high = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h_emptyEve_bcid_adc_high[i_layer]->Draw("COLZ");
        c_emptyEve_bcid_adc_high->Write();
        delete c_emptyEve_bcid_adc_high;
    }

    tree_out->Write();
    fout->Close();
    #pragma endregion
    
    return 1;
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