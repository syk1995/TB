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
    ecal.TDC(datalist, output);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "TDC execution time: " << elapsed.count() << " seconds" << std::endl;
    return 1;
}
int ECAL::TDC(string datalist, string output){
    //initialize datalist & histograms
    #pragma region
    vector<string> runs;
    vector<string> rootFiles;
    if(ReadDatalist(datalist,runs,rootFiles)!=1){
        cerr << "Error: Unable to read datalist" << endl;
        return 0;
    }
    //init_histograms
    h_layer_hitnum = new TH2F("h_layer_hitnum","Layer Hit Number",Layer_No,0,Layer_No,31,-1,30);
    FormatData(h_layer_hitnum,"Layer","Hit Number","Number of hits",1.1,1.1,1.1);
    
    for(int i_layer = 0; i_layer < Layer_No; i_layer++){
        Evt_num[i_layer]=0;
        
        hist_name = Form("h_pedestal_layer%d",i_layer);
        h_pedestal[i_layer] = new TH1F(hist_name.c_str(),hist_name.c_str(),5000,0,5000);
        FormatData(h_pedestal[i_layer],"Pedestal [ADC]","Number of hits",1.1,1.1);

        hist_name = Form("h_bcid_layer%d",i_layer);
        h_bcid[i_layer] = new TH1F(hist_name.c_str(),hist_name.c_str(),5000,0,5000);
        FormatData(h_bcid[i_layer],"BCID","Number of events",1.1,1.1);

        hist_name = Form("h_ADC_all_layer%d",i_layer);
        h_ADC[i_layer][0] = new TH1F(hist_name.c_str(),hist_name.c_str(),5000,0,5000);
        FormatData(h_ADC[i_layer][0],"ADC [ADC]","Number of hits",1.1,1.1);

        hist_name = Form("h_ADC_layer%d",i_layer);
        h_ADC[i_layer][1] = new TH1F(hist_name.c_str(),hist_name.c_str(),5000,0,5000);
        FormatData(h_ADC[i_layer][1],"ADC [ADC]","Number of hits",1.1,1.1);

        hist_name = Form("h_hitnum_layer%d",i_layer);
        h_hitnum[i_layer] = new TH1F(hist_name.c_str(),hist_name.c_str(),100,0,100);
        FormatData(h_hitnum[i_layer],"Hit Number","Number of hits",1.1,1.1);

        hist_name = Form("h_TDC_layer%d_BFSelection",i_layer);
        h_TDC[i_layer][0] = new TH1F(hist_name.c_str(),hist_name.c_str(),5000,0,5000);
        FormatData(h_TDC[i_layer][0],"Output [TDC]","Number of hits",1.1,1.1);

        hist_name = Form("h_TDC_layer%d_AFSelection",i_layer);
        h_TDC[i_layer][1] = new TH1F(hist_name.c_str(),hist_name.c_str(),5000,0,5000);
        FormatData(h_TDC[i_layer][1],"Output [TDC]","Number of hits",1.1,1.1);
   
        hist_name = Form("h_TDC_ref_layer%d",i_layer);
        h_TDC_ref[i_layer] = new TH1F(hist_name.c_str(),hist_name.c_str(),10000,-5000,5000);
        FormatData(h_TDC_ref[i_layer],"Output [TDC]","Number of hits",1.1,1.1);

        hist_name = Form("HitMap_high_Chip_Channel_layer%d",i_layer);
        hitmap_high[i_layer][0] = new TH2F(hist_name.c_str(),hist_name.c_str(),Chip_No,0,Chip_No,Channel_No,0,Channel_No);
        FormatData(hitmap_high[i_layer][0],"Chip","Channel","Number of hits",1.1,1.1,1.1);

        hist_name = Form("HitMap_high_XY_layer%d",i_layer);
        hitmap_high[i_layer][1] = new TH2F(hist_name.c_str(),hist_name.c_str(),Cell_No,RangeX[0]-CellSize/2.,RangeX[1]+CellSize/2.,Cell_No,RangeY[0]-CellSize/2.,RangeY[1]+CellSize/2);
        FormatData(hitmap_high[i_layer][1],"X [mm]","Y [mm]","Number of hits",1.1,1.1,1.1);

        hist_name = Form("h_TDC_E_layer%d",i_layer);
        h_TDC_E[i_layer][0] = new TH2F(hist_name.c_str(),hist_name.c_str(),10000,-5000,5000,5000,0,5000);
        FormatData(h_TDC_E[i_layer][0],"Output [TDC]","Energy depostion [ADC]","Number of hits",1.1,1.1,1.1);
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
        for(int i_entry = 0; i_entry < tree->GetEntries() && i_entry<10000; i_entry++){
            if(i_entry%1000==0)cout<<"Entry "<<i_entry<<" in "<<tree->GetEntries()<<endl;
            Clear();
            TDC_Clear();
            b_MIP = true;
            bool b_hit=false;
            tree->GetEntry(i_entry);               
            //Fill histograms
            for(int i_hit=0; i_hit<_hitbit_high_ptr->size(); i_hit++){
                hitbit_high=_hitbit_high_ptr->at(i_hit);
                hitbit_low=_hitbit_low_ptr->at(i_hit);
                layer=_layer_ptr->at(i_hit);
                chip=_chip_ptr->at(i_hit);
                sca=_sca_ptr->at(i_hit);
                channel=_channel_ptr->at(i_hit);
                adc_high=_adc_high_ptr->at(i_hit);
                adc_low=_adc_low_ptr->at(i_hit);
                X = Pos[layer][chip][channel][0];
                Y = Pos[layer][chip][channel][1];
                Z = Pos[layer][chip][channel][2];
                if(hitbit_high!=hitbit_low){
                    cout<<"Abnormal Hitbit Hitbit high: "<<hitbit_high<<" Hitbit low: "<<hitbit_low<<" "<<_layer_ptr->at(i_hit)<<" "<<_chip_ptr->at(i_hit)<<" "<<_sca_ptr->at(i_hit)<<" "<<_channel_ptr->at(i_hit)<<endl;
                }
                if(hitbit_high>0){
                    if(layer!=8 && layer!=11 && layer!=12){cout<<"Abnormal layer "<<layer<<" chip "<<chip<<" channel "<<channel<<endl;}
                    hitmap_high[layer][0]->Fill(chip,channel);
                    hitmap_high[layer][1]->Fill(X,Y);
                    h_bcid[layer]->Fill(_bcid[layer][chip][sca]);
                    b_hit=true;
                }
                int ID = layer*10000+chip*100+channel;
                if(find(ID_select.begin(), ID_select.end(), ID) == ID_select.end())continue;
                if(hitbit_high==0){
                    h_pedestal[layer]->Fill(adc_low);
                    continue;
                }
                Evt_num[layer]++;
                h_TDC[layer][0]->Fill(adc_high);
                TDC_layer[layer][0] = X;
                TDC_layer[layer][1] = Y;
                TDC_layer[layer][2] = Z;
                TDC_layer[layer][3] = adc_high;
                TDC_layer[layer][4] = adc_low;
                TDC_layer[layer][5] = _bcid[layer][chip][sca];
            }
            //Use the selected channel
            if(!b_hit)cout<<"No hit in this event "<<i_entry<<endl;
            bool b_event = true;
            int bcid_cur;
            for(int i=0; i<ID_select.size(); i++){
                int ID = ID_select[i];
                layer = ID/10000;
                chip = (ID%10000)/100;
                channel = ID%100;
                if(TDC_layer[layer][3]==-1){
                    b_event = false;
                    break;
                }
                if(i==0) bcid_cur = TDC_layer[layer][5];
                if(TDC_layer[layer][5]!=bcid_cur){
                    b_event = false;
                    break;
                }
            }
            if(!b_event)continue;
            TDC_ref = TDC_layer[11][3]; 
            for(int i_layer=0; i_layer<Layer_No; i_layer++){
                if(TDC_layer[i_layer][3]==-1)continue;
                h_TDC[i_layer][1]->Fill(TDC_layer[i_layer][3]);
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
    
    h_layer_hitnum->Write();
    for(int i_layer = 0; i_layer < Layer_No; i_layer++){
        fout->cd();
        fout->mkdir(Form("Layer_%d",i_layer));
        fout->cd(Form("Layer_%d",i_layer));

        h_pedestal[i_layer]->Write();
        h_bcid[i_layer]->Write();
        h_TDC[i_layer][0]->Write();
        h_TDC[i_layer][1]->Write();

        hist_name = Form("h_TDC_ref_layer%d",i_layer);
        TCanvas *c_TDC_ref = new TCanvas(hist_name.c_str());
        h_TDC_ref[i_layer]->Write();
        h_TDC_ref[i_layer]->Draw();
        c_TDC_ref->Write();
        delete c_TDC_ref;

        hist_name = Form("C_HitMap_high_Chip_Channel_layer%d",i_layer);
        TCanvas *c_hitmap_chip = new TCanvas(hist_name.c_str());
        //hitmap_high[i_layer][0]->Write();
        hitmap_high[i_layer][0]->Draw("colz");
        c_hitmap_chip->Write();
        delete c_hitmap_chip;
        max_bin = hitmap_high[i_layer][0]->GetMaximumBin();
        hitmap_high[i_layer][0]->GetBinXYZ(max_bin, binx, biny, binz);
        cout<<"Layer "<<i_layer<<" Chip "<<binx<<" Channel "<<biny<<" Statistics "<<hitmap_high[i_layer][0]->GetBinContent(binx,biny)<<" "<<Evt_num[i_layer]<<endl;

        hist_name = Form("C_HitMap_high_XY_layer%d",i_layer);
        TCanvas *c_hitmap_xy = new TCanvas(hist_name.c_str());
        //hitmap_high[i_layer][1]->Write();
        hitmap_high[i_layer][1]->Draw("colz");
        //c_hitmap_xy->Print(pdfFileName.c_str());
        c_hitmap_xy->Write();
        delete c_hitmap_xy;

    }
    c->Print((pdfFileName + "]").c_str());
    fout->Close();
    #pragma endregion
    
    return 1;
}