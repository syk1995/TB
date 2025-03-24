#include "ECAL.h"

ECAL::ECAL(string Mapfile){
    std::cout << "Initializing ECAL object..." << std::endl;
    std::cout << "Layer Number: " << Layer_No << std::endl;
    std::cout << "Chip Number: " << Chip_No << std::endl;
    std::cout << "SCA Number: " << SCA_No << std::endl;
    std::cout << "Channel Number: " << Channel_No << std::endl;
    _n_slboards=0;
    _acqNumber=0;
    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        _slot[i_layer]=0;
        _slboard_id[i_layer]=0;
        _startACQ[i_layer]=0;
        _rawTSD[i_layer]=0;
        _TSD[i_layer]=0;
        _rawAVDD0[i_layer]=0;
        _rawAVDD1[i_layer]=0;
        _AVDD0[i_layer]=0;
        _AVDD1[i_layer]=0;
        for(int i_chip=0; i_chip<Chip_No; i_chip++){
            _numCol[i_layer][i_chip]=0;
            _chipId[i_layer][i_chip]=0;
            for(int i_sca=0; i_sca<SCA_No; i_sca++){
                _bcid[i_layer][i_chip][i_sca] = 0;
                _corrected_bcid[i_layer][i_chip][i_sca] = 0;
                _badbcid[i_layer][i_chip][i_sca] = 0;
                _nhits[i_layer][i_chip][i_sca] = 0;
                for(int i_channel=0; i_channel<Channel_No; i_channel++){
                    _adc_low[i_layer][i_chip][i_sca][i_channel] = 0;
                    _adc_high[i_layer][i_chip][i_sca][i_channel] = 0;
                    _autogainbit_low[i_layer][i_chip][i_sca][i_channel]=0;
                    _autogainbit_high[i_layer][i_chip][i_sca][i_channel]=0;
                    _hitbit_low[i_layer][i_chip][i_sca][i_channel]=0;
                    _hitbit_high[i_layer][i_chip][i_sca][i_channel]=0;
                }
            }
        }
    }
    //Read Map
    #pragma region
    ifstream fmap(Mapfile);
    if(!fmap){
        std::cerr << "Error opening map file: " << Mapfile << std::endl;
    }
    cout<<"Map file: "<< Mapfile<<std::endl;
    int layer,chip, channel;
    double x, y, z;
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
    std::cout << "ECAL object initialized successfully!" << std::endl;
}
ECAL::~ECAL(){}

void ECAL::Clear(){
    _n_slboards=0;
    _acqNumber=0;
    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        _slot[i_layer]=0;
        _slboard_id[i_layer]=0;
        _startACQ[i_layer]=0;
        _rawTSD[i_layer]=0;
        _TSD[i_layer]=0;
        _rawAVDD0[i_layer]=0;
        _rawAVDD1[i_layer]=0;
        _AVDD0[i_layer]=0;
        _AVDD1[i_layer]=0;
        for(int i_chip=0; i_chip<Chip_No; i_chip++){
            _numCol[i_layer][i_chip]=0;
            _chipId[i_layer][i_chip]=0;
            for(int i_sca=0; i_sca<SCA_No; i_sca++){
                _bcid[i_layer][i_chip][i_sca] = 0;
                _corrected_bcid[i_layer][i_chip][i_sca] = 0;
                _badbcid[i_layer][i_chip][i_sca] = 0;
                _nhits[i_layer][i_chip][i_sca] = 0;
                for(int i_channel=0; i_channel<Channel_No; i_channel++){
                    _adc_low[i_layer][i_chip][i_sca][i_channel] = 0;
                    _adc_high[i_layer][i_chip][i_sca][i_channel] = 0;
                    _autogainbit_low[i_layer][i_chip][i_sca][i_channel]=0;
                    _autogainbit_high[i_layer][i_chip][i_sca][i_channel]=0;
                    _hitbit_low[i_layer][i_chip][i_sca][i_channel]=0;
                    _hitbit_high[i_layer][i_chip][i_sca][i_channel]=0;
                }
            }
        }
    }
}
int ECAL::ReadDatalist(string datalist, vector<string>&runs, vector<string>&rootFiles){
    ifstream infile(datalist);
    if (!infile) {
        cerr << "Error: Cannot open datalist file: " << datalist << endl;
        return 0;
    }
    string line,run_name,file_name;
    while (getline(infile, line)){
        istringstream iss(line);
        if(iss >> run_name >> file_name){
            runs.push_back(run_name);
            rootFiles.push_back(file_name);
        }
        else{cerr << "Error: Failed to parse line: " << line << endl;}
    }
    infile.close();
    cout << "Found " << rootFiles.size() << " ROOT files in "<<datalist << endl;
    return 1;
}

int ECAL::ReadTree(TTree *tree){
    // Read variables from the tree
    tree->SetBranchAddress("adc_low",_adc_low);
    tree->SetBranchAddress("adc_high",_adc_high);
    tree->SetBranchAddress("autogainbit_low",_autogainbit_low);
    tree->SetBranchAddress("autogainbit_high",_autogainbit_high);
    tree->SetBranchAddress("hitbit_high",_hitbit_high);
    tree->SetBranchAddress("hitbit_low",_hitbit_low);
    return 1;
}
