
int noise(){
    TRandom3 rng(0);
    double signal_pre,signal_cur;
    double threshold_range[2]={-20, 20};
    double threshold_bin=20;
    double GausPara[2]={0, 3}; 
    int n_samples = 100000;
    TGraph *g = new TGraph();
    g->SetTitle("Noise Simulation;Threshold;Number of samples above threshold");
    for(int i_thr=0; i_thr<threshold_bin; i_thr++){
        double threshold = threshold_range[0] + i_thr * (threshold_range[1] - threshold_range[0]) / threshold_bin;
        int n_above_threshold = 0;
        signal_pre = rng.Gaus(GausPara[0], GausPara[1]); 
        for (int i_entry = 0; i_entry < n_samples; ++i_entry) {
            signal_cur = rng.Gaus(GausPara[0], GausPara[1]);
            if (signal_cur > threshold && signal_pre < threshold) {
                n_above_threshold++;
            }
            signal_pre = signal_cur;
        }
        cout << "Threshold: " << threshold << ", Samples above threshold: " << n_above_threshold << endl;
        g->SetPoint(i_thr, threshold, n_above_threshold);
    }
    g->SetMarkerStyle(20);
    g->Draw("AP");
    return 1;    
}