#!/bin/bash
run=90764
EXE=BIF_Scan
source /home/llr/ilc/shi/code/TB_2025_3/script_yukun/env.sh
BIN_DIR=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/bin
Mapfile=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/mapping/All_layers_mapping.txt
datalist=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/BIF_Scan/datalist/run_90764.txt
outputfile=/data_ilc/flc/shi/TB_2025_3/Results/BIF/Scan_90764/BIF_90764_B0_H7.root
BIF_offset=0
HCAL_offset=7

${BIN_DIR}/${EXE} ${Mapfile} ${datalist} ${outputfile} ${BIF_offset} ${HCAL_offset}
