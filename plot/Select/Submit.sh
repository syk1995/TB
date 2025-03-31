#!/bin/bash
#Position_Scan
ranges=(
    "90623-90646"
    "90651-90704"
    "90722-90725"
)
#TDC
ranges=(
    "90748-90751"
    "90760-90767"
)
SubmitDir=${PWD}
ENV=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/env.sh
BIN_DIR=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/plot/bin
Mapfile=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/plot/mapping/All_layers_mapping.txt
List_DIR=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/plot/Select/datalist
OUT_DIR=/home/llr/ilc/shi/data/TB_2025_3/TB_Data/selected_root/TDC
for range in "${ranges[@]}"; do
    IFS='-' read -r start end <<< "$range"
    for (( run=start; run<=end; run++ )); do
        datalist=${List_DIR}/run_${run}.txt
        outputfile=${OUT_DIR}/selected_${run}.root
        DIR=${SubmitDir}/Submit/run_${run}
        mkdir -p ${DIR}
        #cp ${PWD}/Select.sh ${DIR}/
        #sed -i "s/run=[0-9]\+/run=${run}/" ${DIR}/Select.sh
        cd ${DIR}
        echo "#!/bin/bash
run=${run}
EXE=Select
source ${ENV}
BIN_DIR=${BIN_DIR}
Mapfile=${Mapfile}
datalist=${datalist}
outputfile=${outputfile}

\${BIN_DIR}/\${EXE} \${Mapfile} \${datalist} \${outputfile}"> ${DIR}/Select.sh
        t3submit -short Select.sh
    done
done
cd ${SubmitDir}