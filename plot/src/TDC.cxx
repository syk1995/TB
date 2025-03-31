#include "ECAL.h"
#include "TDC.h"
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
    for(int i_layer = 0; i_layer < Layer_No; i_layer++){
        hist_name = Form("h_TDC_layer%d",i_layer);
        h_TDC[i_layer] = new TH1F(hist_name.c_str(),hist_name.c_str(),5000,0,5000);
        FormatData(h_TDC[i_layer],"TDC [TDC]","Number of hits",1.1,1.1);
   
        hist_name = Form("h_TDC_ref_layer%d",i_layer);
        h_TDC_ref[i_layer] = new TH1F(hist_name.c_str(),hist_name.c_str(),10000,-5000,5000);
        FormatData(h_TDC_ref[i_layer],"TDC [TDC]","Number of hits",1.1,1.1);

        hist_name = Form("HitMap_high_Chip_Channel_layer%d",i_layer);
        hitmap_high[i_layer][0] = new TH2F(hist_name.c_str(),hist_name.c_str(),Chip_No,0,Chip_No,Channel_No,0,Channel_No);
        FormatData(hitmap_high[i_layer][0],"Chip","Channel","Number of hits",1.1,1.1,1.1);

        hist_name = Form("HitMap_high_XY_layer%d",i_layer);
        hitmap_high[i_layer][1] = new TH2F(hist_name.c_str(),hist_name.c_str(),Cell_No,RangeX[0]-CellSize/2.,RangeX[1]+CellSize/2.,Cell_No,RangeY[0]-CellSize/2.,RangeY[1]+CellSize/2);
        FormatData(hitmap_high[i_layer][1],"X [mm]","Y [mm]","Number of hits",1.1,1.1,1.1);

        hist_name = Form("h_TDC_E_layer%d",i_layer);
        h_TDC_E[i_layer][0] = new TH2F(hist_name.c_str(),hist_name.c_str(),10000,-5000,5000,5000,0,5000);
        FormatData(h_TDC_E[i_layer][0],"TDC [TDC]","Energy depostion [ADC]","Number of hits",1.1,1.1,1.1);
        hist_name = Form("h_TDC_E_ref_layer%d",i_layer);
        h_TDC_E[i_layer][1] = new TH2F(hist_name.c_str(),hist_name.c_str(),10000,-5000,5000,5000,0,5000);
        FormatData(h_TDC_E[i_layer][1],"TDC_ref [TDC]","Energy depostion [ADC]","Number of hits",1.1,1.1,1.1);
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
            TDC_Clear();
            tree->GetEntry(i_entry);
            //Fill histograms
            for(int i_hit=0; i_hit<_hitbit_high_ptr->size(); i_hit++){
                hitbit_high=_hitbit_high_ptr->at(i_hit);
                hitbit_low=_hitbit_low_ptr->at(i_hit);
                if(hitbit_high!=hitbit_low){
                    cout<<"Abnormal Hitbit Hitbit high: "<<hitbit_high<<" Hitbit low: "<<hitbit_low<<" "<<_layer_ptr->at(i_hit)<<" "<<_chip_ptr->at(i_hit)<<" "<<_sca_ptr->at(i_hit)<<" "<<_channel_ptr->at(i_hit)<<endl;
                }
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
                //exclude events with multiple hits in a single layer
                if(TDC_layer[layer][3]!=-1) continue;
                TDC_layer[layer][0] = X;
                TDC_layer[layer][1] = Y;
                TDC_layer[layer][2] = Z;
                TDC_layer[layer][3] = adc_high;
                TDC_layer[layer][4] = adc_low;
                hitmap_high[layer][0]->Fill(chip,channel);
                hitmap_high[layer][1]->Fill(X,Y);
            }
            if(TDC_layer[11][3]==-1 || TDC_layer[12][3]==-1) continue; //layer11 as ref layer
            if(abs(TDC_layer[11][0]-TDC_layer[12][0])>2*CellSize || abs(TDC_layer[11][1]-TDC_layer[12][1])>2*CellSize)continue;//close hit required
            TDC_ref = TDC_layer[11][3]; 
            for(int i_layer=0; i_layer<Layer_No; i_layer++){
                if(TDC_layer[i_layer][3]==-1)continue;
                h_TDC[i_layer]->Fill(TDC_layer[i_layer][3]);
                h_TDC_ref[i_layer]->Fill(TDC_layer[i_layer][3]-TDC_ref);
                h_TDC_E[i_layer][0]->Fill(TDC_layer[i_layer][3],TDC_layer[i_layer][4]);
                h_TDC_E[i_layer][1]->Fill(TDC_layer[i_layer][3]-TDC_ref,TDC_layer[i_layer][4]);
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
    //Yukun_Style();
    rootlogon();
    gROOT->SetBatch(kTRUE);
    
    for(int i_layer = 0; i_layer < Layer_No; i_layer++){
        fout->cd();
        fout->mkdir(Form("Layer_%d",i_layer));
        fout->cd(Form("Layer_%d",i_layer));
        
        hist_name = Form("h_TDC_layer%d",i_layer);
        TCanvas *c_TDC = new TCanvas(hist_name.c_str());
        h_TDC[i_layer]->Write();
        h_TDC[i_layer]->Draw();
        c_TDC->Print(pdfFileName.c_str());
        c_TDC->Write();
        delete c_TDC;

        hist_name = Form("h_TDC_ref_layer%d",i_layer);
        TCanvas *c_TDC_ref = new TCanvas(hist_name.c_str());
        h_TDC_ref[i_layer]->Write();
        h_TDC_ref[i_layer]->Draw();
        c_TDC_ref->Print(pdfFileName.c_str());
        c_TDC_ref->Write();
        delete c_TDC_ref;

        hist_name = Form("C_HitMap_high_Chip_Channel_layer%d",i_layer);
        TCanvas *c_hitmap_chip = new TCanvas(hist_name.c_str());
        //hitmap_high[i_layer][0]->Write();
        hitmap_high[i_layer][0]->Draw("colz");
        c_hitmap_chip->Print(pdfFileName.c_str());
        c_hitmap_chip->Write();
        delete c_hitmap_chip;

        hist_name = Form("C_HitMap_high_XY_layer%d",i_layer);
        TCanvas *c_hitmap_xy = new TCanvas(hist_name.c_str());
        //hitmap_high[i_layer][1]->Write();
        hitmap_high[i_layer][1]->Draw("colz");
        c_hitmap_xy->Print(pdfFileName.c_str());
        c_hitmap_xy->Write();
        delete c_hitmap_xy;

        hist_name = Form("h_TDC_E_layer%d",i_layer);
        TCanvas *c_TDC_E = new TCanvas(hist_name.c_str(),hist_name.c_str(),1500,1200);
        //h_TDC_E[i_layer][0]->Write();
        h_TDC_E[i_layer][0]->Draw("colz");
        c_TDC_E->Print(pdfFileName.c_str());
        c_TDC_E->Write();
        delete c_TDC_E;

        hist_name = Form("h_TDC_E_ref_layer%d",i_layer);
        TCanvas *c_TDC_E_ref = new TCanvas(hist_name.c_str(),hist_name.c_str(),1500,1200);
        //h_TDC_E[i_layer][1]->Write();
        h_TDC_E[i_layer][1]->Draw("colz");
        c_TDC_E_ref->Print(pdfFileName.c_str());
        c_TDC_E_ref->Write();
        delete c_TDC_E_ref;
    }
    c->Print((pdfFileName + "]").c_str());
    fout->Close();
    #pragma endregion
    
    return 1;
}