#!/bin/bash
EXE=TDC
run=90767
source /home/llr/ilc/shi/code/TB_2025_3/script_yukun/env.sh
BIN_DIR=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/plot/bin
Mapfile=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/plot/mapping/All_layers_mapping.txt
#fev10_chip_channel_x_y_mapping.txt
#fev11_cob_rotate_chip_channel_x_y_mapping.txt
datalist=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/plot/TDC/datalist/run_${run}.txt
outputfile=/data_ilc/flc/shi/TB_2025_3/Results/TDC/TDC_${run}.root

${BIN_DIR}/${EXE} ${Mapfile} ${datalist} ${outputfile}