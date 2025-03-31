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
    
    h_layerprofile = new TH1F("h_layerprofile","layer profile", Layer_No, 0, Layer_No);
    FormatData(h_layerprofile,"Layer ID","Number of events",1.1,1.1);

    h2_layerprofile = new TH2F("h2_layerprofile","layer profile", Layer_No, 0, Layer_No, 1000, 0, 1000);
    FormatData(h2_layerprofile,"Layer ID","High gain output[ADC]","Number of hits",1.1,1.1,1.1);

    for(int i_layer = 0; i_layer < Layer_No; i_layer++){
        hist_name = Form("HitMap_high_Chip_Channel_layer%d",i_layer);
        hitmap_high[i_layer][0] = new TH2F(hist_name.c_str(),hist_name.c_str(),Chip_No,0,Chip_No,Channel_No,0,Channel_No);
        FormatData(hitmap_high[i_layer][0],"Chip ID","Channel ID","Number of hits",1.1,1.1,1.1);
    
        hist_name = Form("HitMap_high_XY_layer%d",i_layer);
        hitmap_high[i_layer][1] = new TH2F(hist_name.c_str(),hist_name.c_str(),Cell_No,RangeX[0]-CellSize/2.,RangeX[1]+CellSize/2.,Cell_No,RangeY[0]-CellSize/2.,RangeY[1]+CellSize/2);
        FormatData(hitmap_high[i_layer][1],"X [mm]","Y [mm]","Number of hits",1.1,1.1,1.1);

        hist_name = Form("h_hitno_channel_layer%d",i_layer);
        h_hitno_channel[i_layer] = new TH1F(hist_name.c_str(),hist_name.c_str(),50000,0,50000);
        FormatData(h_hitno_channel[i_layer],"Number of hits","Number of channels",1.1,1.1);
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
        TTree *tree = (TTree*)fin->Get(treename_selected.c_str());
        if (!tree) {
            cerr << "Error: Cannot find TTree "<<treename_selected <<" in " << rootFile << endl;
            fin->Close();
            continue;
        }
        ReadTree(tree,BranchNames_hitmap,BranchDesciptions_hitmap);
        for(int i_entry = 0; i_entry < tree->GetEntries() && i_entry>-1; i_entry++){
            if(i_entry%1000==0)cout<<"Entry "<<i_entry<<" in "<<tree->GetEntries()<<endl;
            Clear();
            HitMap_Clear();
            tree->GetEntry(i_entry);
            //Fill histograms
            for(int i_hit=0; i_hit<_hitbit_high_ptr->size(); i_hit++){
                hitbit_high=_hitbit_high_ptr->at(i_hit);
                hitbit_low=_hitbit_low_ptr->at(i_hit);
                if(hitbit_high!=hitbit_low){
                    cout<<"Abnormal Hitbit Hitbit high: "<<hitbit_high<<" Hitbit low: "<<hitbit_low<<" "<<_layer_ptr->at(i_hit)<<" "<<_chip_ptr->at(i_hit)<<" "<<_sca_ptr->at(i_hit)<<" "<<_channel_ptr->at(i_hit)<<endl;
                }
                h_hitbit_high->Fill(hitbit_high);
                h_hitbit_low->Fill(hitbit_low);
                if(hitbit_high==0)continue;
                layer=_layer_ptr->at(i_hit);
                chip=_chip_ptr->at(i_hit);
                sca=_sca_ptr->at(i_hit);
                channel=_channel_ptr->at(i_hit);
                adc_high=_adc_high_ptr->at(i_hit);
                adc_low=_adc_low_ptr->at(i_hit);
                X = Pos[layer][chip][channel][0];
                Y = Pos[layer][chip][channel][1];
                Z = Pos[layer][chip][channel][2];
                EvtNum_layer[layer]++;
                hitmap_high[layer][0]->Fill(chip,channel);
                hitmap_high[layer][1]->Fill(X,Y);
                h2_layerprofile->Fill(layer,adc_high);
            }
            for(int i_layer=0; i_layer<Layer_No; i_layer++){
                if(EvtNum_layer[i_layer]>0) h_layerprofile->Fill(i_layer);
            }
        }
        fin->Close();
        cout<<"Read over "<<rootFile<<endl;
    }
    #pragma endregion
    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        for(int binX=1; binX<hitmap_high[i_layer][0]->GetNbinsX(); binX++){
            for(int binY=1; binY<hitmap_high[i_layer][0]->GetNbinsY(); binY++){
                h_hitno_channel[i_layer]->Fill(hitmap_high[i_layer][0]->GetBinContent(binX,binY));
            }
        }
    }
        

    //Write histograms to output file
    #pragma region
    TFile *fout = new TFile(output.c_str(), "RECREATE");
    string pdfFileName = output.substr(0, output.find_last_of(".")) + ".pdf";
    TCanvas *c = new TCanvas();
    c->Print((pdfFileName + "[").c_str());
    //Yukun_Style();
    rootlogon();
    gROOT->SetBatch(kTRUE);
    //hitbit_high
    TCanvas *c_hitbit_high = new TCanvas("C_hitbit_high","C_hitbit_high",800,600);
    h_hitbit_high->Draw();
    h_hitbit_high->Write();    
    c_hitbit_high->Print(pdfFileName.c_str());
    delete c_hitbit_high;
    //hitbit_low
    TCanvas *c_hitbit_low = new TCanvas("C_hitbit_low","C_hitbit_low",800,600);
    h_hitbit_low->Draw();
    h_hitbit_low->Write();
    c_hitbit_low->Print(pdfFileName.c_str());
    delete c_hitbit_low;
    //layer profile
    TCanvas *c_layerprofile = new TCanvas("C_layerprofile","C_layerprofile",800,600);
    h_layerprofile->Draw();
    h_layerprofile->Write();
    c_layerprofile->Print(pdfFileName.c_str());
    //h2_layerprofile
    TCanvas *c2_layerprofile = new TCanvas("C2_layerprofile","C2_layerprofile",800,600);
    h2_layerprofile->Draw("colz");
    h2_layerprofile->Write();
    c2_layerprofile->Print(pdfFileName.c_str());
    //HitMap
    for(int i_layer = 0; i_layer < Layer_No; i_layer++){
        fout->cd();
        fout->mkdir(Form("Layer_%d",i_layer));
        fout->cd(Form("Layer_%d",i_layer));
        
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

        hist_name = Form("C_hitno_channel_layer%d",i_layer);
        TCanvas *c_hitno_channel = new TCanvas(hist_name.c_str());
        //h_hitno_channel[i_layer]->Write();
        h_hitno_channel[i_layer]->Draw();
        c_hitno_channel->Print(pdfFileName.c_str());
        h_hitno_channel[i_layer]->Write();
        c_hitno_channel->Write();
        delete c_hitno_channel;
    }
    c->Print((pdfFileName + "]").c_str());
    fout->Close();
    #pragma endregion
    
    return 1;
}