#!/bin/bash
Script_Dir="/home/llr/ilc/shi/code/TB_2025_3/script_yukun/conversion"
Input_Dir="/home/llr/ilc/shi/data/tb-desy/2025/TB2025-03/beamData/Run_Data"
output_Dir="/home/llr/ilc/shi/data/shi/TB_2025_3/TB_Data/convertedfiles"
run_start=90623
run_end=90630


echo "make sure in SLBraw2ROOT.cc: _eudaq= false;"
for (( run=$run_start; run<=$run_end; run++ ))
do
    run_number=$(printf "%06d" $run)
    run_name="eudaq_run_${run_number}"
    folder="${Input_Dir}/${run_name}"
    if [ -d "$folder" ]; then
        #echo "Found folder: $folder"
        folder="${output_Dir}/${run_name}"
        #rm -r $folder
        if [[ -d "$folder" ]]; then
            echo "Output directory for $run_name already exists, skipping..."
            continue
        fi
        mkdir -p $folder

        find "${Input_Dir}/${run_name}" -type f -name "*raw.bin*" | while read input_file; do
            output_file=$(basename $input_file)
            output_file="${output_Dir}/${run_name}/${output_file}.root"
            root -l -q ${Script_Dir}/Convert.cc\(\"${input_file}\",true,\"${output_file}\"\)
        done
    else
        echo "Folder $folder not found"
    fi
done

