#include "ECAL.h"
using namespace std;
ECAL::ECAL(string Mapfile, string Maskfile){
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
                    _TCC[i_layer][i_chip][i_sca][i_channel] = 0;
                }
            }
        }
    }
    _layer_ptr=new std::vector<int>;
    _chip_ptr=new std::vector<int>;
    _sca_ptr=new std::vector<int>;
    _channel_ptr=new std::vector<int>;
    _adc_high_ptr=new std::vector<int>;
    _adc_low_ptr=new std::vector<int>;
    _autogainbit_high_ptr=new std::vector<int>;
    _autogainbit_low_ptr=new std::vector<int>;
    _hitbit_high_ptr=new std::vector<int>;
    _hitbit_low_ptr=new std::vector<int>;
    BranchMap_int_v["layer"] = _layer_ptr;
    BranchMap_int_v["chip"] = _chip_ptr;
    BranchMap_int_v["sca"] = _sca_ptr;
    BranchMap_int_v["channel"] = _channel_ptr;
    BranchMap_int_v["adc_low"] = _adc_low_ptr;
    BranchMap_int_v["adc_high"] = _adc_high_ptr;
    BranchMap_int_v["autogainbit_low"] = _autogainbit_low_ptr;
    BranchMap_int_v["autogainbit_high"] = _autogainbit_high_ptr;
    BranchMap_int_v["hitbit_high"] = _hitbit_high_ptr;
    BranchMap_int_v["hitbit_low"] = _hitbit_low_ptr;
    BranchMap_int["acqNumber"] = &_acqNumber;
    BranchMap_int["bcid"] = (int*)_bcid;
    BranchMap_int["corrected_bcid"] = (int*)_corrected_bcid;
    BranchMap_int["badbcid"] = (int*)_badbcid;
    BranchMap_int["adc_low"] = (int*)_adc_low;
    BranchMap_int["adc_high"] = (int*)_adc_high;
    BranchMap_int["autogainbit_low"] = (int*)_autogainbit_low;
    BranchMap_int["autogainbit_high"] = (int*)_autogainbit_high;
    BranchMap_int["hitbit_high"] = (int*)_hitbit_high;
    BranchMap_int["hitbit_low"] = (int*)_hitbit_low;
    //Read Position Map
    #pragma region
    ifstream fmap(Mapfile);
    if(!fmap){
        std::cerr << "Error opening map file: " << Mapfile << std::endl;
    }
    cout<<"Map file: "<< Mapfile<<std::endl;
    int layer,chip, channel;
    double x,y,z;
    RangeX[0]=9999;RangeX[1]=-9999;
    RangeY[0]=9999;RangeY[1]=-9999;
    getline(fmap,str_buffer);
    while (fmap >> layer >> chip >> channel >> x >> y >>z) {
        _Pos[layer][chip][channel][0] = x;
        _Pos[layer][chip][channel][1] = y;
        _Pos[layer][chip][channel][2] = z;
        if(x==-999||y==-999) continue;
        if(x<RangeX[0]) RangeX[0]=x;
        if(x>RangeX[1]) RangeX[1]=x;
        if(y<RangeY[0]) RangeY[0]=y;
        if(y>RangeY[1]) RangeY[1]=y;
        if(x>1000) cout<<layer<<" "<<chip<<" "<<channel<<" "<<x<<" "<<y<<" "<<z<<endl;
    }
    cout<<"Map X range "<<RangeX[0]<<" "<<RangeX[1]<<endl;
    cout<<"Map Y range "<<RangeY[0]<<" "<<RangeY[1]<<endl;
    fmap.close();
    #pragma endregion
    //Read Mask file
    #pragma region
    ifstream fmask(Maskfile);
    if(!fmask){
        std::cerr << "Error opening mask file: " << Maskfile << std::endl;
        cout<<"initialize without mask file"<<endl;
    }
    else{
        getline(fmask,str_buffer);
        while (fmask >> layer >> chip >> channel) {
            _mask_channels.push_back(layer*10000+chip*100+channel);
        }
    }
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
    Clear_Vector();
}
void ECAL::Clear_Vector(){
    if(_layer_ptr!=nullptr)_layer_ptr->clear();
    if(_chip_ptr!=nullptr)_chip_ptr->clear();
    if(_sca_ptr!=nullptr)_sca_ptr->clear();
    if(_channel_ptr!=nullptr)_channel_ptr->clear();
    if(_adc_high_ptr!=nullptr)_adc_high_ptr->clear();
    if(_adc_low_ptr!=nullptr)_adc_low_ptr->clear();
    if(_autogainbit_high_ptr!=nullptr)_autogainbit_high_ptr->clear();
    if(_autogainbit_low_ptr!=nullptr)_autogainbit_low_ptr->clear();
    if(_hitbit_high_ptr!=nullptr)_hitbit_high_ptr->clear();
    if(_hitbit_low_ptr!=nullptr)_hitbit_low_ptr->clear();
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

int ECAL::ReadTree(TTree *tree,std::vector<std::string> BranchNames, std::vector<std::string> BranchDescriptors){
    //Read variables from the tree
    tree->SetBranchStatus("*", 0);
    string branch_name;
    for (size_t i = 0; i < BranchNames.size(); i++){
        branch_name = BranchNames[i];
        if(BranchDescriptors[i]=="int" || BranchDescriptors[i].find("/I") != std::string::npos){
            if(BranchMap_int.find(branch_name)==BranchMap_int.end()){
                cerr << "Read Tree: Cannot find branch: " << branch_name << endl;
                continue;
            }
            cout<<"Read branch:"<<branch_name<<endl;
            tree->SetBranchStatus(branch_name.c_str(), 1);
            tree->SetBranchAddress(branch_name.c_str(),BranchMap_int[branch_name]);
        }
        if(BranchDescriptors[i]=="vector<int>*"){
            if(BranchMap_int_v.find(branch_name)==BranchMap_int_v.end()){
                cerr << "Read Tree: Cannot find branch: " << branch_name << endl;
                continue;
            }
            cout<<"Read branch:"<<branch_name<<endl;
            tree->SetBranchStatus(branch_name.c_str(), 1);
            tree->SetBranchAddress(branch_name.c_str(),&BranchMap_int_v[branch_name]);
        }
    }
    return 1;
}

int ECAL::WriteTree(TTree *tree,std::vector<std::string> BranchNames, std::vector<std::string> BranchDescriptors){
    //Read variables from the tree
    string branch_name;
    for (size_t i = 0; i < BranchNames.size(); i++){
        branch_name = BranchNames[i];
        if(tree->GetBranch(branch_name.c_str())){
            cout<<"Output branch Merge: "<<branch_name<<endl;
            continue;
        }
        else{
            cout<<"Output branch Add: "<<branch_name<<endl;
        }
        if (BranchDescriptors[i].find("/I") != std::string::npos){
            if(BranchMap_int.find(branch_name)==BranchMap_int.end()){
                cerr << "Write TTree: Cannot find branch: " << branch_name << endl;
                continue;
            }
            tree->Branch(branch_name.c_str(),BranchMap_int[branch_name],BranchDescriptors[i].c_str());
        }
        if(BranchDescriptors[i]=="vector<int>*"){
            if(BranchMap_int_v.find(branch_name)==BranchMap_int_v.end()){
                cerr << "Write TTree: Cannot find branch: " << branch_name << endl;
                continue;
            }
            tree->Branch(branch_name.c_str(),BranchMap_int_v[branch_name]);
        }
    }
    return 1;
}