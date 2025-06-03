#!/bin/bash

RawDIR="/data_ilc/flc/tb-desy/2025/TB2025-03/beamData/eudaq_data/raw"
RootDir=/grid_mnt/vol_home/llr/ilc/shi/data/TB_2025_3/TB_Data/selected_root/TDC
BIFDir=/grid_mnt/vol_home/llr/ilc/shi/data/TB_2025_3/TB_Data/slcio2root/BIF
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
#ranges=("90623-90623")
for range in "${ranges[@]}"; do
    IFS='-' read -r start end <<< "$range"
    for (( run=start; run<=end; run++ )); do
        raw_file="${RawDIR}/raw_siwecal_${run}.raw"
        run_name=$(printf "%06d" $run)
        root_file=${RootDir}/selected_${run}.root
        BIF_file=${BIFDir}/ROC_run_${run_name}_tp.root
        if [[ ! -f "${raw_file}" ]]; then
            echo "${raw_file} not found"
            continue
        fi
        if [[ ! -f "${root_file}" ]]; then
            echo "${root_file} not found"
            continue
        fi
        if [[ ! -f "${BIF_file}" ]]; then
            echo "${BIF_file} not found"
            continue
        fi
        raw_size=$(du -sb "${raw_file}" | cut -f1)
        root_size=$(du -sb "${root_file}" | cut -f1)
        BIF_size=$(du -sb "${BIF_file}" | cut -f1)
        if [[ "$raw_size" -le 1048576 ]]; then
            echo " ${raw_file} : $raw_size <= 1MB"
            continue
        fi
        if [[ "$root_size" -le 1048576 ]]; then
            echo " ${root_file} : $root_size <= 1MB"
            continue
        fi
        if [[ "$BIF_size" -le 1048576 ]]; then
            echo " ${BIF_file} : $BIF_size <= 1MB"
            continue
        fi
        echo ${run} ${root_file} ${BIF_file} > datalist/run_${run}.txt
        #echo $raw_size"MB "$root_size"MB"
    done
done

