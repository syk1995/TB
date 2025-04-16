#!/bin/bash
run=90764
EXE=Select
source /home/llr/ilc/shi/code/TB_2025_3/script_yukun/env.sh
BIN_DIR=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/bin
Mapfile=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/mapping/All_layers_mapping.txt
Maskfile=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/mask/TB_2025_3.txt
datalist=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/Select/datalist/run_${run}.txt
outputfile=/home/llr/ilc/shi/data/TB_2025_3/TB_Data/selected_root/selected_${run}.root

${BIN_DIR}/${EXE} ${Mapfile} ${Maskfile} ${datalist} ${outputfile}
