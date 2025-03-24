const int Layer_No = 15;
const int Chip_No = 16;
const int Channel_No = 64;

#include <iostream>

int ReadMap(ifstream &f_map, float Pos_temp[Chip_No][Channel_No][3]){
    int chip, channel;
    double x, y, x0, y0;
    string str_buffer;
    getline(f_map,str_buffer);
    while (f_map >> chip >> x0 >> y0 >> channel >> x >> y) {
        Pos_temp[chip][channel][0] = x;
        Pos_temp[chip][channel][1] = y;
    }
    return 1;
}

int createmapping(){
    string mapping_name[2]={"fev10_chip_channel_x_y_mapping.txt","fev11_cob_rotate_chip_channel_x_y_mapping.txt"};
    vector<int> mapping_layers[2]={{8},{11,12}};
    string output_name = "All_layers_mapping.txt";
    ifstream f_map[2];
    float Pos[Layer_No][Chip_No][Channel_No][3];
    float Pos_temp[Chip_No][Channel_No][3];
    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        for(int i_chip=0; i_chip<Chip_No; i_chip++){
            for(int i_channel=0; i_channel<Channel_No; i_channel++){
                Pos[i_layer][i_chip][i_channel][0] = -999;
                Pos[i_layer][i_chip][i_channel][1] = -999;
                Pos[i_layer][i_chip][i_channel][2] = -999;
            }
        }
    }
    for(int i=0; i<2; i++){
        f_map[i].open(mapping_name[i]);
        if(!f_map[i].is_open()){
            cerr << "Error opening file: " << mapping_name[i] << endl;
            return 1;
        }
        ReadMap(f_map[i], Pos_temp);
        for(int i_map=0; i_map<mapping_layers[i].size(); i_map++){
            int layer = mapping_layers[i][i_map];
            for(int i_chip=0; i_chip<Chip_No; i_chip++){
                for(int i_channel=0; i_channel<Channel_No; i_channel++){
                    Pos[layer][i_chip][i_channel][0] = Pos_temp[i_chip][i_channel][0];
                    Pos[layer][i_chip][i_channel][1] = Pos_temp[i_chip][i_channel][1];
                }
            }
        }            
    }
    ofstream f_out;
    f_out.open(output_name);
    if(!f_out.is_open()){
        cerr << "Error opening file: " << output_name << endl;
        return 1;
    }
    f_out<<"Layer\tChip\tChannel\tX\tY\tZ"<<endl;
    for(int i_layer=0; i_layer<Layer_No; i_layer++){
        for(int i_chip=0; i_chip<Chip_No; i_chip++){
            for(int i_channel=0; i_channel<Channel_No; i_channel++){
                f_out<<i_layer<<"\t"<<i_chip<<"\t"<<i_channel<<"\t"<<Pos[i_layer][i_chip][i_channel][0]<<"\t"<<Pos[i_layer][i_chip][i_channel][1]<<"\t"<<Pos[i_layer][i_chip][i_channel][2]<<endl;
            }
        }
    }
    return 1;
}