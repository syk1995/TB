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
    h2_TDC_2channel = new TH2F("h_TDC_2channel","h_TDC_2channel",4096,0,4096,4096,0,4096);
    FormatData(h2_TDC_2channel,"TDC channel 1","TDC channel 2","Number of hits",1.1,1.1,1.1);

    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        Evt_num[i_layer]=0;
        
        hist_name = Form("h_pedestal_low_layer%d",i_layer);
        h_pedestal[i_layer][0] = new TH1F(hist_name.c_str(),hist_name.c_str(),5000,0,5000);
        FormatData(h_pedestal[i_layer][0],"Low gain output [ADC]","Number of hits",1.1,1.1);

        hist_name = Form("h_pedestal_high_layer%d",i_layer);
        h_pedestal[i_layer][1] = new TH1F(hist_name.c_str(),hist_name.c_str(),5000,0,5000);
        FormatData(h_pedestal[i_layer][1],"High gain output [TDC]","Number of hits",1.1,1.1);
  
        hist_name = Form("h_BIF_TDC_layer%d",i_layer);
        h_BIF_TDC[i_layer] = new TH2F(hist_name.c_str(),hist_name.c_str(),256,0,256,5000,0,5000);
        FormatData(h_BIF_TDC[i_layer],"BIF TDC","SPIROC TDC","Number of hits",1.1,1.1,1.1);

        hist_name = Form("HitMap_high_Chip_Channel_layer%d",i_layer);
        hitmap_high[i_layer][0] = new TH2F(hist_name.c_str(),hist_name.c_str(),Chip_No,-0.5,Chip_No-0.5,Channel_No,-0.5,Channel_No-0.5);
        FormatData(hitmap_high[i_layer][0],"Chip","Channel","Number of hits",1.1,1.1,1.1);

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
        TTree *tree = (TTree*)fin->Get(treename_TDC.c_str());
        if (!tree) {
            cerr << "Error: Cannot find TTree "<<treename_selected <<" in " << rootFile << endl;
            fin->Close();
            continue;
        }
        ReadTree(tree,BranchNames_TDC,BranchDesciptions_TDC);
        for(int i_entry = 0; i_entry < tree->GetEntries() && i_entry<Read_EventNum; i_entry++){
            if(i_entry%1000==0)cout<<"Entry "<<i_entry<<" in "<<tree->GetEntries()<<endl;
            Clear();
            TDC_Clear();
            tree->GetEntry(i_entry);              
            for(int i_hit=0; i_hit<_hitbit_high_ptr->size(); i_hit++){
                hitbit_high=_hitbit_high_ptr->at(i_hit);
                hitbit_low=_hitbit_low_ptr->at(i_hit);
                layer=_layer_ptr->at(i_hit);
                chip=_chip_ptr->at(i_hit);
                sca=_sca_ptr->at(i_hit);
                channel=_channel_ptr->at(i_hit);
                adc_high=_adc_high_ptr->at(i_hit);
                adc_low=_adc_low_ptr->at(i_hit);
                X = _Pos[layer][chip][channel][0];
                Y = _Pos[layer][chip][channel][1];
                Z = _Pos[layer][chip][channel][2];
                HitID = layer*10000+chip*100+channel;
                if(hitbit_high==0){
                    h_pedestal[layer][0]->Fill(adc_low);
                    h_pedestal[layer][1]->Fill(adc_high);
                    if(find(selected_ID.begin(), selected_ID.end(), HitID) != selected_ID.end()){
                        hit_vec.push_back({X,Y,Z,adc_high,adc_low,_corrected_bcid[layer][chip][sca],sca,layer*10000+chip*100+channel});
                    }
                    continue;
                }
                if(bcid_cur==-1)bcid_cur = _bcid[layer][chip][sca];
                if(fabs(_bcid[layer][chip][sca] - bcid_cur)>1){
                    cout<<"Mis matched bcid in entry "<<i_entry<<" "<<_bcid[layer][chip][sca]<<" "<<bcid_cur<<" layer "<<layer<<" chip "<<chip<<" sca "<<sca<<endl;
                    continue;
                }
                Evt_num[layer]++;
                hitmap_high[layer][0]->Fill(chip,channel);
                hitmap_high[layer][1]->Fill(X,Y);
            }
            if(hit_vec.size()==2){
                for(int i_hit=0; i_hit<hit_vec.size(); i_hit++){
                    h2_TDC_2channel->Fill(hit_vec[i_hit].ADC,hit_vec[1-i_hit].ADC);
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
    //Yukun_Style();
    rootlogon();
    gROOT->SetBatch(kTRUE);
    
    hist_name = "C_h2_TDC_2channel";
    TCanvas *c_h2_TDC_2channel = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
    h2_TDC_2channel->Draw("colz");
    c_h2_TDC_2channel->Write();
    delete c_h2_TDC_2channel;

    for(int i=0; i<selected_layers.size(); i++){
        int i_layer = selected_layers[i];
        fout->cd();
        fout->mkdir(Form("Layer_%d",i_layer));
        fout->cd(Form("Layer_%d",i_layer));

        h_pedestal[i_layer][0]->Write();
        h_pedestal[i_layer][1]->Write();

        hist_name = Form("C_BIF_TDC_layer%d",i_layer);
        TCanvas *c_BIF_TDC = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h_BIF_TDC[i_layer]->Draw("colz");
        c_BIF_TDC->Write();
        delete c_BIF_TDC;

        hist_name = Form("C_HitMap_high_Chip_Channel_layer%d",i_layer);
        TCanvas *c_hitmap_chip = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        //hitmap_high[i_layer][0]->Write();
        hitmap_high[i_layer][0]->Draw("colz");
        c_hitmap_chip->Write();
        delete c_hitmap_chip;
        max_bin = hitmap_high[i_layer][0]->GetMaximumBin();
        hitmap_high[i_layer][0]->GetBinXYZ(max_bin, binx, biny, binz);
        cout<<"Layer "<<i_layer<<" Chip "<<binx<<" Channel "<<biny<<" Statistics "<<hitmap_high[i_layer][0]->GetBinContent(binx,biny)<<" "<<Evt_num[i_layer]<<endl;

        hist_name = Form("C_HitMap_high_XY_layer%d",i_layer);
        TCanvas *c_hitmap_xy = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
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