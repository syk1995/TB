#!/bin/bash
EXE=BIF_Build
run=90764
BIF_offset=-2
HCAL_offset=-3
source /home/llr/ilc/shi/code/TB_2025_3/script_yukun/env.sh
#source /home/llr/ilc/shi/key4hep/env.sh
BIN_DIR=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/bin
Mapfile=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/mapping/All_layers_mapping.txt
#fev10_chip_channel_x_y_mapping.txt
#fev11_cob_rotate_chip_channel_x_y_mapping.txt
datalist=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/BIF_Scan/datalist/run_${run}.txt
OUT_DIR=/data_ilc/flc/shi/TB_2025_3/Results/BIF/Build_${run}
mkdir -p ${OUT_DIR}
outputfile=${OUT_DIR}/BIF_${run}.root
${BIN_DIR}/${EXE} ${Mapfile} ${datalist} ${outputfile} ${BIF_offset} ${HCAL_offset} 