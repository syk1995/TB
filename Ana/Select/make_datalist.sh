#!/bin/bash

RawDIR="/data_ilc/flc/tb-desy/2025/TB2025-03/beamData/eudaq_data/raw"
RootDir="/home/llr/ilc/shi/data/TB_2025_3/TB_Data/raw2root/TDC"
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
        root_file=${RootDir}/siwecal_${run}.root
        if [[ ! -f "${raw_file}" ]]; then
            echo "${raw_file} not found"
            continue
        fi
        if [[ ! -f "${root_file}" ]]; then
            echo "${root_file} not found"
            continue
        fi
        raw_size=$(du -sb "${raw_file}" | cut -f1)
        root_size=$(du -sb "${root_file}" | cut -f1)
        if [[ "$raw_size" -le 1048576 ]]; then
            echo " ${raw_file} : $raw_size <= 1MB"
            continue
        fi
        if [[ "$root_size" -le 1048576 ]]; then
            echo " ${root_file} : $root_size <= 1MB"
            continue
        fi
        raw_size=$(( $raw_size / (1048576) ))
        root_size=$(( $root_size / (1048576) ))
        echo ${run} ${root_file} > datalist/run_${run}.txt
        #echo $raw_size"MB "$root_size"MB"
    done
done

