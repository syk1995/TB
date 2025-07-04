#!/bin/bash
source /home/llr/ilc/shi/key4hep/env.sh
Script_Dir="/home/llr/ilc/shi/code/TB_2025_3/script_yukun/conversion"
Input_Dir="/data_ilc/flc/tb-desy/2025/TB2025-03/beamData/eudaq_data"
output_Dir="/home/llr/ilc/shi/data/TB_2025_3/TB_Data/slcio2root/TDC"
#Position_Scan
ranges=(
    "90623-90646"
    "90651-90704"
    "90722-90725"
)
#TDC
ranges=(
    "90748-90751"
    "90760-90769"
)
ranges=("90764-90764")
for range in "${ranges[@]}"; do
    IFS='-' read -r start end <<< "$range"
    for (( run=start; run<=end; run++ )); do
        run_name=$(printf "%06d" ${run})
        run_name="ROC_run_${run_name}_tp"
        input_file=${Input_Dir}/${run_name}.slcio
        if [ ! -f "$input_file" ]; then
            echo "File not found: $input_file, skipping..."
            continue
        fi
        output_file="${output_Dir}/${run_name}.root"
        if [ -f "$output_file" ]; then
            echo "Output already exists: $output_file, skipping..."
            #continue
        fi
        root -l -q ${Script_Dir}/slcio2root.cxx\(\"${input_file}\",\"${output_file}\"\)
    done
done
