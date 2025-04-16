#include <iostream>
#include <TFile.h>
#include <TH2F.h>
#include "/home/llr/ilc/shi/code/Plot_style/cepcplotstyle.cxx"

int channel_statistics(){
    using namespace std;
    rootlogon();
    std::string fin_name = "/grid_mnt/vol_home/llr/ilc/shi/data/TB_2025_3/Results/PositionScan/All.root";
    const int Layer_No = 15;
    const int Chip_No = 16;
    const int SCA_No = 15;
    const int Channel_No = 64;
    const int Cell_No = 32;
    const float CellSize=5.5;
    int layer, chip, channel;
    vector<int> layer_v={8,11,12};
    TH1F *h_hitnum_channel[Layer_No];

    //Read from fin
    #pragma region
    TFile *fin = TFile::Open(fin_name.c_str(), "READ");
    if (!fin || !fin->IsOpen()) {
        std::cerr << "Error opening file: " << fin_name << std::endl;
        return 1;
    }
    for(int i_layer=0; i_layer<layer_v.size(); i_layer++){
        layer = layer_v[i_layer];
        h_hitnum_channel[layer] = (TH1F*)fin->Get(Form("Layer_%d/h_hitno_channel_layer%d",layer,layer));
        h_hitnum_channel[layer]->SetDirectory(0);
    }
    #pragma endregion

    TCanvas *C_hitnum_channel;
    C_hitnum_channel = new TCanvas("C_hitnum_channel","C_hitnum_channel",800,600);
    TLegend *leg_hitnum_channel;
    leg_hitnum_channel = new TLegend(0.6,0.6,0.9,0.9);
    for(int i_layer=0; i_layer<layer_v.size(); i_layer++){
        layer = layer_v[i_layer];
        h_hitnum_channel[layer]->Rebin(100);
        h_hitnum_channel[layer]->GetXaxis()->SetRangeUser(0,20000);
        h_hitnum_channel[layer]->SetLineColor(i_layer+1);
        h_hitnum_channel[layer]->Draw("same");
        leg_hitnum_channel->AddEntry(h_hitnum_channel[layer],Form("Layer %d",layer),"l");
    }
    leg_hitnum_channel->Draw();
    return 1;
}