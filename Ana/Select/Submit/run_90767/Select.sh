#!/bin/bash
run=90767
EXE=Select
source /home/llr/ilc/shi/code/TB_2025_3/script_yukun/env.sh
BIN_DIR=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/plot/bin
Mapfile=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/plot/mapping/All_layers_mapping.txt
datalist=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/plot/Select/datalist/run_90767.txt
outputfile=/home/llr/ilc/shi/data/TB_2025_3/TB_Data/selected_root/TDC/selected_90767.root

${BIN_DIR}/${EXE} ${Mapfile} ${datalist} ${outputfile}
