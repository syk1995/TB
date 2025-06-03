#!/bin/bash
run=90764
EXE=BIF
source /home/llr/ilc/shi/code/TB_2025_3/script_yukun/env.sh
BIN_DIR=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/bin
Mapfile=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/mapping/All_layers_mapping.txt
datalist=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/BIF/datalist/run_90764.txt
outputfile=/data_ilc/flc/shi/TB_2025_3/Results/BIF/BIF_90764_B6_H0.root
BIF_offset=6
HCAL_offset=0

${BIN_DIR}/${EXE} ${Mapfile} ${datalist} ${outputfile} ${BIF_offset} ${HCAL_offset}
