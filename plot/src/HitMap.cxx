#include "ECAL.h"
#include "Hitmap.h"
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
        
    h_hitbit_low = new TH1F("h_hitbit_low","Hitbit Low", 2200, -1100, 100);
    FormatData(h_hitbit_low,"Hitbit low","Number of hits",1.1,1.1);
        
    for(int i_layer = 0; i_layer < Layer_No; i_layer++){
        hist_name = Form("HitMap_high_Chip_Channel_layer%d",i_layer);
        hitmap_high[i_layer][0] = new TH2F(hist_name.c_str(),hist_name.c_str(),Chip_No,0,Chip_No-1,Channel_No,0,Channel_No-1);
        FormatData(hitmap_high[i_layer][0],"Chip ID","Channel ID","Number of hits",1.1,1.1,1.1);
    
        hist_name = Form("HitMap_high_XY_layer%d",i_layer);
        hitmap_high[i_layer][1] = new TH2F(hist_name.c_str(),hist_name.c_str(),Cell_No,RangeX[0]-CellSize/2.,RangeX[1]+CellSize/2.,Cell_No,RangeY[0]-CellSize/2.,RangeY[1]+CellSize/2);
        FormatData(hitmap_high[i_layer][1],"X [mm]","Y [mm]","Number of hits",1.1,1.1,1.1);
    }
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
        TTree *tree = (TTree*)fin->Get("siwecaldecoded");
        if (!tree) {
            cerr << "Error: Cannot find TTree in " << rootFile << endl;
            fin->Close();
            continue;
        }
        ReadTree(tree);
        for(int i_entry = 0; i_entry < tree->GetEntries() && i_entry>-1; i_entry++){
            if(i_entry%1000==0)cout<<"Entry "<<i_entry<<" in "<<tree->GetEntries()<<endl;
            Clear();
            tree->GetEntry(i_entry);
            //Fill histograms
            for(int i_layer = 0; i_layer < Layer_No; i_layer++){
                for(int i_chip = 0; i_chip < Chip_No; i_chip++){
                    for(int i_sca = 0; i_sca < SCA_No; i_sca++){
                        for(int i_channel =0; i_channel < Channel_No; i_channel++){
                            h_hitbit_high->Fill(_hitbit_high[i_layer][i_chip][i_sca][i_channel]);
                            h_hitbit_low->Fill(_hitbit_low[i_layer][i_chip][i_sca][i_channel]);
                            if(_hitbit_high[i_layer][i_chip][i_sca][i_channel]!=1) continue;
                            hitmap_high[i_layer][0]->Fill(i_chip,i_channel);
                            hitmap_high[i_layer][1]->Fill(Pos[i_layer][i_chip][i_channel][0],Pos[i_layer][i_chip][i_channel][1]);  
                        }
                    }
                }
            }

        }
        fin->Close();
        cout<<"Read over "<<rootFile<<endl;
    }
    #pragma endregion

    //Write histograms to output file
    #pragma region
    TFile *fout = new TFile(output.c_str(), "RECREATE");
    string pdfFileName = output.substr(0, output.find_last_of(".")) + ".pdf";
    TCanvas *c = new TCanvas();
    c->Print((pdfFileName + "[").c_str());
    rootlogon();
    gROOT->SetBatch(kTRUE);
    //hitbit_high
    TCanvas *c_hitbit_high = new TCanvas("C_hitbit_high","C_hitbit_high",800,600);
    h_hitbit_high->Write();    
    h_hitbit_high->Draw();
    c_hitbit_high->Print(pdfFileName.c_str());
    delete c_hitbit_high;
    //hitbit_low
    TCanvas *c_hitbit_low = new TCanvas("C_hitbit_low","C_hitbit_low",800,600);
    h_hitbit_low->Write();
    h_hitbit_low->Draw();
    c_hitbit_low->Print(pdfFileName.c_str());
    delete c_hitbit_low;
    //HitMap
    for(int i_layer = 0; i_layer < Layer_No; i_layer++){
        hist_name = Form("C_HitMap_high_Chip_Channel_layer%d",i_layer);
        TCanvas *c_hitmap_chip = new TCanvas(hist_name.c_str());
        //hitmap_high[i_layer][0]->Write();
        hitmap_high[i_layer][0]->Draw("colz");
        c_hitmap_chip->Print(pdfFileName.c_str());
        c_hitmap_chip->Write();
        delete c_hitmap_chip;

        hist_name = Form("C_HitMap_high_XY_layer%d",i_layer);
        TCanvas *c_hitmap_XY = new TCanvas(hist_name.c_str());
        //hitmap_high[i_layer][1]->Write();
        hitmap_high[i_layer][1]->Draw("colz");
        c_hitmap_XY->Print(pdfFileName.c_str());
        c_hitmap_XY->Write();
        delete c_hitmap_XY;
    }
    c->Print((pdfFileName + "]").c_str());
    fout->Close();
    #pragma endregion
    
    return 1;
}