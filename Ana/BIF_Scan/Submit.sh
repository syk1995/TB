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
ranges=(
    "90764-90764"
)
EXE=BIF_Scan
ENV=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/env.sh
BIN_DIR=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/bin
Mapfile=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/mapping/All_layers_mapping.txt
List_DIR=/home/llr/ilc/shi/code/TB_2025_3/script_yukun/Ana/${EXE}/datalist
SubmitDir=${PWD}
for range in "${ranges[@]}"; do
    IFS='-' read -r start end <<< "$range"
    for (( run=start; run<=end; run++ )); do
        OUT_DIR=/data_ilc/flc/shi/TB_2025_3/Results/BIF/Scan_${run}
        mkdir -p ${OUT_DIR}
        for (( BIF_offset=-0; BIF_offset<=0; BIF_offset++ )); do
            for (( HCAL_offset=-10; HCAL_offset<=10; HCAL_offset++ )); do
                datalist=${List_DIR}/run_${run}.txt
                outputfile=${OUT_DIR}/BIF_${run}_B${BIF_offset}_H${HCAL_offset}.root
                DIR=${SubmitDir}/Submit/run_${run}/B${BIF_offset}_H${HCAL_offset}
                mkdir -p ${DIR}
                cd ${DIR}
                echo "#!/bin/bash
run=${run}
EXE=${EXE}
source ${ENV}
BIN_DIR=${BIN_DIR}
Mapfile=${Mapfile}
datalist=${datalist}
outputfile=${outputfile}
BIF_offset=${BIF_offset}
HCAL_offset=${HCAL_offset}

\${BIN_DIR}/\${EXE} \${Mapfile} \${datalist} \${outputfile} \${BIF_offset} \${HCAL_offset}" > ${DIR}/BIF.sh

                #t3submit -short BIF.sh
            done
        done
    done
done
cd ${SubmitDir}