#include <iomanip>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <map>
#include <algorithm>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TString.h>
#include <iostream>
#include <string>
#include "/home/llr/ilc/shi/code/Plot_style/cepcplotstyle.cxx"

int GetMaxPoint(TGraph* g, double& xmax, double& ymax) {
    int n = g->GetN();
    double* x = g->GetX();
    double* y = g->GetY();

    int imax = 0;
    ymax = y[0];
    xmax = x[0];

    for (int i = 1; i < n; ++i) {
        if (y[i] > ymax) {
            ymax = y[i];
            xmax = x[i];
            imax = i;
        }
    }
    return imax;
}

int Scan_cycle(){
    rootlogon();
    gROOT->SetBatch(kTRUE);
    string run_name = "90764";
    string type_name = "BIF"; //BIF or "HCAL"
    string input_dir = "/grid_mnt/vol_home/llr/ilc/shi/data/TB_2025_3/Results/BIF/Scan_90764";
    string intput_name;
    string output_name = input_dir + "/Scan_" + type_name +"_cycle.root";
    TFile *fin, *fout;
    int Scan_range[2]= {-10, 10};
    const int Scanning_No = Scan_range[1] - Scan_range[0] + 1;
    int BIF_cycleNr_offset = 0;
    int HCAL_cycleNr_offset = 0;
    double xmax, ymax;
    int correct_offset = 0;
    int event_num_max = 0; 
    TGraph *g_Offset[Scanning_No];
    TCanvas *c_Offset[Scanning_No];
    TGraph *g_Offset_Max;
    g_Offset_Max = new TGraph();
    g_Offset_Max->SetName("g_Offset_Max");
    g_Offset_Max->SetTitle("Offset Max");
    FormatData(g_Offset_Max, "Cycle Offset", "Max Number of Events", 1.1, 1.1);
    
    TMultiGraph *mg_Offset = new TMultiGraph();



    
    for(int i =0; i < Scanning_No; i++){
        if(type_name == "BIF") {
            BIF_cycleNr_offset = Scan_range[0] + i;
        }
        else if(type_name == "HCAL") {
            HCAL_cycleNr_offset = Scan_range[0] + i;
        } else {
            cerr << "Error: Unknown type_name " << type_name << endl;
            return 0;
        }
        intput_name = input_dir + "/BIF_" + run_name + Form("_B%d_H%d", BIF_cycleNr_offset, HCAL_cycleNr_offset) + ".root";
        fin = TFile::Open(intput_name.c_str());
        if (!fin || fin->IsZombie()) {
            cerr << "Error: Cannot open ROOT file: " << intput_name << endl;
            continue;
        }
        cout << "Processing file: " << intput_name << endl;
        g_Offset[i] = (TGraph*)fin->Get(Form("g_%s_Offset", type_name.c_str()));
        fin->Close();
    }
    fout = new TFile(output_name.c_str(), "RECREATE");
    if (!fout || fout->IsZombie()) {
        cerr << "Error: Cannot open output file: " << output_name << endl;
        return 0;
    }
    fout->cd();

    for(int i = 0; i < Scanning_No; i++){
        //g_Offset[i]->Write();
        c_Offset[i] = new TCanvas(Form("c_Offset_%d", i), Form("Offset %d", i), 800, 600);
        g_Offset[i]->Draw("ALP");
        c_Offset[i]->Write();
        GetMaxPoint(g_Offset[i], xmax, ymax);
        g_Offset_Max->SetPoint(i, Scan_range[0] + i, ymax);
        cout << "Max for offset " << Scan_range[0] + i << ": " << ymax << endl;
        if(ymax > event_num_max) {
            event_num_max = ymax;
            correct_offset = i;
        }
    }
    TCanvas *c_Offset_Max = new TCanvas("c_Offset_Max", "Offset Max", 800, 600);
    g_Offset_Max->Draw("ALP");
    c_Offset_Max->Write();

    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    for(int i = correct_offset -2; i <= correct_offset + 2; i++) {
        if(i < 0 || i >= Scanning_No) continue;
        FormatData(g_Offset[i], "BCID Offset", "Number of Matched Events", 1.1, 1.1);
        g_Offset[i]->SetMarkerColor(i - correct_offset + 3);
        g_Offset[i]->SetLineColor(i - correct_offset + 3);
        mg_Offset->Add(g_Offset[i]);
        legend->AddEntry(g_Offset[i], Form("%s Cycle Offset %d", type_name.c_str(),Scan_range[0] + i), "lp");
    }
    

    TCanvas *c_mg_Offset = new TCanvas("c_mg_Offset", "MultiGraph Offset", 800, 600);
    mg_Offset->SetName("mg_Offset");
    mg_Offset->SetTitle("Offset Comparison; Cycle Offset; Max Number of Events");
    FormatData(mg_Offset, "BCID Offset", "Number of Matched Events", 1.1, 1.1);
    mg_Offset->Draw("ALP");
    legend->Draw();
    c_mg_Offset->Write();

    cout << "Finished processing all files." << endl;
    return 1;
}