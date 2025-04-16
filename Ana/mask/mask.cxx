#include <iostream>
#include <TFile.h>
#include <TH2F.h>
#include "/home/llr/ilc/shi/code/Plot_style/cepcplotstyle.cxx"
int mask(){
    using namespace std;
    std::string fin_name = "/data_ilc/flc/tb-desy/2025/TB2025-03/beamData/Run_Data/eudaq_run_090652/Run_Settings.txt";
    string fmap_name = "/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/mapping/All_layers_mapping.txt";
    string ofstream_name = "/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/mask/mask.txt";
    string fout_name = "/grid_mnt/vol_home/llr/ilc/shi/data/TB_2025_3/Results/mask.root";

    std::string line;
    string hist_name;
    const int Layer_No = 15;
    const int Chip_No = 16;
    const int SCA_No = 15;
    const int Channel_No = 64;
    const int Cell_No = 32;
    const float CellSize=5.5;
    int layer = -1;
    int chip = -1;
    int channel = -1;
    int trigmask = -1;
    float Pos[Layer_No][Chip_No][Channel_No][3];
    float RangeX[2], RangeY[2];
    map<int, int> layer_conversion;
    layer_conversion[0] = 8;
    layer_conversion[1] = 11;
    layer_conversion[2] = 12;
    std::vector<int> maskid;//layer*10000+chip*100+channel

    rootlogon();
    TH2F *hitmap_mask[Layer_No][2];//0 for chip-channel, 1 for xy

    //Read from fin
    #pragma region
    std::ifstream fin(fin_name);
    if (!fin.is_open()) {
        std::cerr << "cant open " << fin_name << std::endl;
        return 1;
    }    
    std::regex asu_regex(R"(ASU:\s*(\d+))");
    std::regex chip_regex(R"(ChipIndex:\s*(\d+))");
    std::regex channel_regex(R"(### Ch:\s*(\d+)\s*TrigMask:\s*(\d+))");
    while (std::getline(fin, line)) {
        std::smatch match;
        if (std::regex_search(line, match, asu_regex)) {
            layer++;
        }
        if (std::regex_search(line, match, chip_regex)) {
            chip = std::stoi(match[1]);
        }
        if (std::regex_search(line, match, channel_regex)) {
            channel = std::stoi(match[1]);
            trigmask = std::stoi(match[2]);
            if (trigmask == 1) {
                //std::cout << "Layer: " << layer << ", Chip: " << chip << ", Ch: " << channel << std::endl;
                maskid.push_back(layer*10000+chip*100+channel);
            }
        }
    }
    fin.close();
    #pragma endregion

    //Read from map file
    #pragma region
    ifstream fmap(fmap_name);
    if(!fmap){
        std::cerr << "Error opening map file: " << fmap_name << std::endl;
    }
    cout<<"Map file: "<< fmap_name<<std::endl;
    double x,y,z;
    RangeX[0]=9999;RangeX[1]=-9999;
    RangeY[0]=9999;RangeY[1]=-9999;
    string str_buffer;
    getline(fmap,str_buffer);
    while (fmap >> layer >> chip >> channel >> x >> y >>z) {
        Pos[layer][chip][channel][0] = x;
        Pos[layer][chip][channel][1] = y;
        Pos[layer][chip][channel][2] = z;
        if(x==-999||y==-999) continue;
        if(x<RangeX[0]) RangeX[0]=x;
        if(x>RangeX[1]) RangeX[1]=x;
        if(y<RangeY[0]) RangeY[0]=y;
        if(y>RangeY[1]) RangeY[1]=y;
    }
    cout<<"Map X range "<<RangeX[0]<<" "<<RangeX[1]<<endl;
    cout<<"Map Y range "<<RangeY[0]<<" "<<RangeY[1]<<endl;
    fmap.close();
    #pragma endregion
       
    //Initialize histograms
    for(int i_layer = 0; i_layer < Layer_No; i_layer++){
        hist_name = Form("HitMap_mask_Chip_Channel_layer%d",i_layer);
        hitmap_mask[i_layer][0] = new TH2F(hist_name.c_str(),hist_name.c_str(),Chip_No,0,Chip_No,Channel_No,0,Channel_No);
        FormatData(hitmap_mask[i_layer][0],"Chip ID","Channel ID","Number of hits",1.1,1.1,1.1);
    
        hist_name = Form("HitMap_mask_XY_layer%d",i_layer);
        hitmap_mask[i_layer][1] = new TH2F(hist_name.c_str(),hist_name.c_str(),Cell_No,RangeX[0]-CellSize/2.,RangeX[1]+CellSize/2.,Cell_No,RangeY[0]-CellSize/2.,RangeY[1]+CellSize/2);
        FormatData(hitmap_mask[i_layer][1],"X [mm]","Y [mm]","Number of hits",1.1,1.1,1.1);
    }
    //Fill histograms
    for(int i_channel=0;i_channel<maskid.size();i_channel++){
        layer = maskid[i_channel]/10000;
        chip = (maskid[i_channel]-layer*10000)/100;
        channel = maskid[i_channel]-layer*10000-chip*100;
        layer = layer_conversion[layer];
        //cout<<layer<<" "<<chip<<" "<<channel<<endl;
        //cout<<Pos[layer][chip][channel][0]<<" "<<Pos[layer][chip][channel][1]<<" "<<Pos[layer][chip][channel][2]<<endl;
        hitmap_mask[layer][0]->Fill(chip,channel);
        hitmap_mask[layer][1]->Fill(Pos[layer][chip][channel][0],Pos[layer][chip][channel][1]);
    }
    //Write histograms to output file
    ofstream fout_stream(ofstream_name);
    if (!fout_stream.is_open()) {
        std::cerr << "Error: Cannot open output file: " << ofstream_name << std::endl;
        return 1;
    }
    fout_stream<<"Mask layer chip channel"<<endl;
    for(int i_channel=0;i_channel<maskid.size();i_channel++){
        layer = maskid[i_channel]/10000;
        chip = (maskid[i_channel]-layer*10000)/100;
        channel = maskid[i_channel]-layer*10000-chip*100;
        fout_stream<<layer_conversion[layer]<<" "<<chip<<" "<<channel<<endl;
    }
    fout_stream.close();

    TFile *fout = new TFile(fout_name.c_str(), "RECREATE");
    if (!fout->IsOpen()) {
        std::cerr << "Error: Cannot open output file: " << fout_name << std::endl;
        return 1;
    }
    gROOT->SetBatch(kTRUE);
    for(int i_layer = 0; i_layer < Layer_No; i_layer++){
        fout->cd();
        fout->mkdir(Form("Layer_%d",i_layer));
        fout->cd(Form("Layer_%d",i_layer));
        TCanvas *c_hitmap_chip = new TCanvas(Form("C_HitMap_mask_Chip_Channel_layer%d",i_layer));
        hitmap_mask[i_layer][0]->Draw("colz");
        hitmap_mask[i_layer][0]->Write();
        c_hitmap_chip->Write();
        delete c_hitmap_chip;
        TCanvas *c_hitmap_xy = new TCanvas(Form("C_HitMap_mask_XY_layer%d",i_layer));
        hitmap_mask[i_layer][1]->Draw("colz");
        hitmap_mask[i_layer][1]->Write();
        c_hitmap_xy->Write();
        delete c_hitmap_xy;
    }
    fout->Close();

    return 1;
}