#!/bin/bash
run=90764
EXE=TDC_Calib
#source /home/llr/ilc/shi/code/TB_2025_3/script_yukun/env.sh
BIN_DIR=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/bin
Mapfile=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/mapping/All_layers_mapping.txt
Maskfile=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/mask/TB_2025_3.txt
datalist=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/Calib/datalist/run_${run}.txt
outputfile=/home/llr/ilc/shi/data/TB_2025_3/Results/Calib/TDC/${run}.root

${BIN_DIR}/${EXE} ${Mapfile} ${Maskfile} ${datalist} ${outputfile}
echo "TDC calibration done"
