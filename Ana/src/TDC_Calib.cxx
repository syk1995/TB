#include "ECAL.h"
#include "TDC_Calib.h"
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
    ecal.TDC_Calib(datalist, output);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;
    return 1;
}
int ECAL::TDC_Calib(string datalist, string output){
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
    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        if(i_layer!=selected_layer) continue;
        hist_name = Form("h_TCCDiff_layer%d_start",i_layer);
        h_TCCDiff[i_layer][0] = new TH1F(hist_name.c_str(),hist_name.c_str(),2000,-1000,1000);
        FormatData(h_TCCDiff[i_layer][0],"Start TDC difference [TDC]","Number of channels",1.1,1.1);
        hist_name = Form("h_TCCDiff_layer%d_end",i_layer);
        h_TCCDiff[i_layer][1] = new TH1F(hist_name.c_str(),hist_name.c_str(),2000,-1000,1000);
        FormatData(h_TCCDiff[i_layer][1],"End TDC difference [TDC]","Number of channels",1.1,1.1);

        hist_name = Form("h_hitmap_ChipChannel_layer%d",i_layer);
        h_hitmap[i_layer][0] = new TH2F(hist_name.c_str(),hist_name.c_str(),Chip_No,-0.5,Chip_No-0.5,Channel_No,-0.5,Channel_No-0.5);
        FormatData(h_hitmap[i_layer][0],"Chip","Channel","Number of hits",1.1,1.1,1.1);

        hist_name = Form("h_hitmap_XY_layer%d",i_layer);
        h_hitmap[i_layer][1] = new TH2F(hist_name.c_str(),hist_name.c_str(),Cell_No,RangeX[0]-CellSize/2.,RangeX[1]+CellSize/2.,Cell_No,RangeY[0]-CellSize/2.,RangeY[1]+CellSize/2);
        FormatData(h_hitmap[i_layer][1],"X [mm]","Y [mm]","Number of hits",1.1,1.1,1.1);

        hist_name = Form("h_TCC_layer%d_ChipChannel",i_layer);
        h2_TCC[i_layer][0] = new TH2F(hist_name.c_str(),hist_name.c_str(),Chip_No,0,Chip_No,Channel_No,0,Channel_No);
        FormatData(h2_TCC[i_layer][0],"Chip","Channel","TCC [TDC/ns]",1.1,1.1,1.1);

        hist_name = Form("h_TCC_layer%d_XY",i_layer);
        h2_TCC[i_layer][1] = new TH2F(hist_name.c_str(),hist_name.c_str(),Cell_No,RangeX[0]-CellSize/2.,RangeX[1]+CellSize/2.,Cell_No,RangeY[0]-CellSize/2.,RangeY[1]+CellSize/2);
        FormatData(h2_TCC[i_layer][1],"X [mm]","Y [mm]","TCC [TDC/ns]",1.1,1.1,1.1);

        hist_name = Form("h_TCC_SCA_layer%d",i_layer);
        h_TCC_SCA[i_layer] = new TH2F(hist_name.c_str(),hist_name.c_str(),4096,0,4096,SCA_No,0,SCA_No);
        FormatData(h_TCC_SCA[i_layer],"TCC [TDC/ns]","SCA","Number of channels",1.1,1.1,1.1);
        for(int i_chip=0; i_chip<Chip_No; i_chip++){
            for(int i_sca=0; i_sca<SCA_No; i_sca++){
                for(int i_channel=0; i_channel<Channel_No; i_channel++){                    
                    hist_name = Form("h_TDC_layer%d_chip%d_sca%d_channel%d_hittag=0",i_layer,i_chip,i_sca,i_channel);
                    h_TDC[i_layer][i_chip][i_sca][i_channel][0] = new TH1F(hist_name.c_str(),hist_name.c_str(),4096,0,4096);
                    FormatData(h_TDC[i_layer][i_chip][i_sca][i_channel][0],"Output [TDC]","Number of hits",1.1,1.1);
                    hist_name = Form("h_TDC_layer%d_chip%d_sca%d_channel%d_hittag=1",i_layer,i_chip,i_sca,i_channel);
                    h_TDC[i_layer][i_chip][i_sca][i_channel][1] = new TH1F(hist_name.c_str(),hist_name.c_str(),4096,0,4096);
                    FormatData(h_TDC[i_layer][i_chip][i_sca][i_channel][1],"Output [TDC]","Number of hits",1.1,1.1);
                    hist_name = Form("h_Cycle_TDC_layer%d_chip%d_sca%d_channel%d",i_layer,i_chip,i_sca,i_channel);
                    h_Cycle_TDC[i_layer][i_chip][i_sca][i_channel][0] = new TH2F(hist_name.c_str(),hist_name.c_str(),4096,0,4096,256,0,4096);
                    FormatData(h_Cycle_TDC[i_layer][i_chip][i_sca][i_channel][0],"BCID","Output [TDC]","Number of hits",1.1,1.1,1.1);
                    hist_name = Form("h_Cycle_TDC_layer%d_chip%d_sca%d_channel%d_1",i_layer,i_chip,i_sca,i_channel);
                    h_Cycle_TDC[i_layer][i_chip][i_sca][i_channel][1] = new TH2F(hist_name.c_str(),hist_name.c_str(),2,-0.5,1.5,256,0,4096);
                    FormatData(h_Cycle_TDC[i_layer][i_chip][i_sca][i_channel][1],"(Cycle+BCID)%2","Output [TDC]","Number of hits",1.1,1.1,1.1);
                }
            }
        }
    }

    TFile *fout = new TFile(output.c_str(), "RECREATE");
    if (!fout || fout->IsZombie()) {
        cerr << "Error: Cannot open output file: " << output << endl;
        return 0;
    }
    string TxtFileName = output.substr(0, output.find_last_of(".")) + ".txt";
    ofstream txtFile(TxtFileName);
    if (!txtFile) {
        cerr << "Error: Cannot open text file: " << TxtFileName << endl;
        return 0;
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
        TTree *tree_in = (TTree*)fin->Get(treename_raw.c_str());
        if (!tree_in) {
            cerr << "Error: Cannot find TTree in " << rootFile << endl;
            fin->Close();
            continue;
        }
        ReadTree(tree_in,BranchNames_raw,BranchDesciptions_raw);
        for(int i_entry = 0; i_entry < tree_in->GetEntries() && i_entry>-1; i_entry++){
            if(i_entry%1000==0)cout<<"Entry "<<i_entry<<" in "<<tree_in->GetEntries()<<endl;
            //Clear variables
            #pragma region
            Clear();
            #pragma endregion
            tree_in->GetEntry(i_entry);
            for(int i_layer = 0; i_layer < Layer_No; i_layer++){
                if(i_layer!=selected_layer) continue;
                for(int i_chip = 0; i_chip < Chip_No; i_chip++){
                    for(int i_sca = 0; i_sca < SCA_No; i_sca++){
                        if(_badbcid[i_layer][i_chip][i_sca] != 0) continue;                        
                        for(int i_channel = 0; i_channel < Channel_No; i_channel++){
                            if(_hitbit_high[i_layer][i_chip][i_sca][i_channel] == 0){
                                h_TDC[i_layer][i_chip][i_sca][i_channel][0]->Fill(_adc_high[i_layer][i_chip][i_sca][i_channel]);
                                h_Cycle_TDC[i_layer][i_chip][i_sca][i_channel][0]->Fill(_bcid[i_layer][i_chip][i_sca],_adc_high[i_layer][i_chip][i_sca][i_channel]);
                                h_Cycle_TDC[i_layer][i_chip][i_sca][i_channel][1]->Fill((_acqNumber+_bcid[i_layer][i_chip][i_sca])%2,_adc_high[i_layer][i_chip][i_sca][i_channel]);
                            }
                            else{
                                h_TDC[i_layer][i_chip][i_sca][i_channel][1]->Fill(_adc_high[i_layer][i_chip][i_sca][i_channel]);
                                h_hitmap[i_layer][0]->Fill(i_chip,i_channel);
                                h_hitmap[i_layer][1]->Fill(_Pos[i_layer][i_chip][i_channel][0],_Pos[i_layer][i_chip][i_channel][1]);}
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
    fout->cd();


    //for(int i_layer=0; i_layer<Layer_No; i_layer++){
    for(int i_layer=selected_layer; i_layer<selected_layer+1; i_layer++){    
        for(int i_chip=0; i_chip<Chip_No; i_chip++){
            for(int i_sca=0; i_sca<SCA_No; i_sca++){
                for(int i_channel=0; i_channel<Channel_No; i_channel++){
                    fout->cd();
                    string dir_name = Form("Layer%d/Chip%d/SCA%d/Channel%d",i_layer,i_chip,i_sca,i_channel);
                    fout->mkdir(dir_name.c_str());
                    fout->cd(dir_name.c_str());
                    h_TDC[i_layer][i_chip][i_sca][i_channel][0]->Write();
                    h_TDC[i_layer][i_chip][i_sca][i_channel][1]->Write();
                    h_Cycle_TDC[i_layer][i_chip][i_sca][i_channel][0]->Write();
                    h_Cycle_TDC[i_layer][i_chip][i_sca][i_channel][1]->Write();
                    _TCC[i_layer][i_chip][i_sca][i_channel] = GetTCC(h_TDC[i_layer][i_chip][i_sca][i_channel][1]);
                    if(i_sca==0){
                        h2_TCC[i_layer][0]->Fill(i_chip,i_channel,_TCC[i_layer][i_chip][i_sca][i_channel]);
                        h2_TCC[i_layer][1]->Fill(_Pos[i_layer][i_chip][i_channel][0],_Pos[i_layer][i_chip][i_channel][1],_TCC[i_layer][i_chip][i_sca][i_channel]);
                        if(h_TDC[i_layer][i_chip][i_sca][i_channel][1]->GetEntries() < EntriesCut)continue;
                        double TDC_start[2], TDC_end[2],peak[2];
                        for(int ii=0; ii<2; ii++){
                            h_TDC[i_layer][i_chip][i_sca][i_channel][ii]->Rebin(16);
                            peak[ii] = h_TDC[i_layer][i_chip][i_sca][i_channel][ii]->GetBinContent(h_TDC[i_layer][i_chip][i_sca][i_channel][ii]->GetMaximumBin());
                            TDC_start[ii] = h_TDC[i_layer][i_chip][i_sca][i_channel][ii]->GetBinCenter(h_TDC[i_layer][i_chip][i_sca][i_channel][ii]->FindFirstBinAbove(peak[ii]*0.25,1,5,-1));
                            TDC_end[ii] = h_TDC[i_layer][i_chip][i_sca][i_channel][ii]->GetBinCenter(h_TDC[i_layer][i_chip][i_sca][i_channel][ii]->FindLastBinAbove(peak[ii]*0.25,1,5,-1));
                        }
                        if(fabs(TDC_start[1]-TDC_start[0])>100 || fabs(TDC_end[1]-TDC_end[0])>100){
                            cout<<"Layer "<<i_layer<<" Chip "<<i_chip<<" SCA "<<i_sca<<" Channel "<<i_channel<<" TCC "<<_TCC[i_layer][i_chip][i_sca][i_channel]<<" TDC_start "<<TDC_start[0]<<" TDC_end "<<TDC_end[0]<<" TDC_start "<<TDC_start[1]<<" TDC_end "<<TDC_end[1]<<endl;
                        }
                        h_TCCDiff[i_layer][0]->Fill(TDC_start[1]-TDC_start[0]);
                        h_TCCDiff[i_layer][1]->Fill(TDC_end[1]-TDC_end[0]);
                    }
                    //cout<<"Layer "<<i_layer<<" Chip "<<i_chip<<" SCA "<<i_sca<<" Channel "<<i_channel<<" TCC "<<_TCC[i_layer][i_chip][i_sca][i_channel]<<endl;
                    h_TCC_SCA[i_layer]->Fill(_TCC[i_layer][i_chip][i_sca][i_channel],i_sca);
                }
            }            
        }
    }

        //Write TCC to text file
        txtFile << "Bunch Crossing Interval: " << Bunch_Crossing_Interval << " ns" << endl;
        txtFile << "Layer Chip SCA Channel TCC [TDC/ns]" << endl;
        for(int i_layer=0; i_layer<Layer_No; i_layer++){
            if(i_layer!=selected_layer) continue;
            for(int i_chip=0; i_chip<Chip_No; i_chip++){
                for(int i_channel=0; i_channel<Channel_No; i_channel++){
                    double TCC_min = 9999;
                    double TCC_max = -9999;
                    for(int i_sca=0; i_sca<SCA_No; i_sca++){
                        txtFile << i_layer << " " << i_chip << " " << i_sca << " " << i_channel << " " << _TCC[i_layer][i_chip][i_sca][i_channel] << endl;
                    }
                }
            }
        }

    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        if(i_layer!=selected_layer) continue;
        fout->cd(Form("Layer%d",i_layer));

        hist_name = Form("C_Start_TDC_Diff_layer%d",i_layer);
        TCanvas *c_TCCDiff0 = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h_TCCDiff[i_layer][0]->Draw();
        c_TCCDiff0->Write();
        delete c_TCCDiff0;

        hist_name = Form("C_End_TDC_Diff_layer%d",i_layer);
        TCanvas *c_TCCDiff1 = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h_TCCDiff[i_layer][1]->Draw();
        c_TCCDiff1->Write();
        delete c_TCCDiff1;

        hist_name = Form("C_h_hitmap_chipchannel_layer%d",i_layer);
        TCanvas *c_hitmap_chip = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h_hitmap[i_layer][0]->Draw("colz");
        c_hitmap_chip->Write();
        delete c_hitmap_chip;
        max_bin = h_hitmap[i_layer][0]->GetMaximumBin();
        h_hitmap[i_layer][0]->GetBinXYZ(max_bin, binx, biny, binz);
        cout<<"Layer "<<i_layer<<" Chip "<<binx<<" Channel "<<biny<<" Statistics "<<h_hitmap[i_layer][0]->GetBinContent(binx,biny)<<endl;

        hist_name = Form("C_hitmap_XY_layer%d",i_layer);
        TCanvas *c_hitmap = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h_hitmap[i_layer][1]->Draw("colz");
        c_hitmap->Write();
        delete c_hitmap;

        hist_name = Form("h_TCC_layer%d_ChipChannel",i_layer);
        TCanvas *c_TCC_chip = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h2_TCC[i_layer][0]->Draw("colz");
        c_TCC_chip->Write();

        hist_name = Form("h_TCC_layer%d_XY",i_layer);
        TCanvas *c_TCC_xy = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h2_TCC[i_layer][1]->Draw("colz");
        c_TCC_xy->Write();

        hist_name = Form("h_TCC_SCA_layer%d",i_layer);
        TCanvas *c_TCC_SCA = new TCanvas(hist_name.c_str(),hist_name.c_str(), 800, 600);
        h_TCC_SCA[i_layer]->Draw("colz");
        c_TCC_SCA->Write();
    }
    txtFile.close();
    fout->Close();
    #pragma endregion
    
    return 1;
}
