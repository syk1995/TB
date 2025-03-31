#!/bin/bash
Data_DIR=/data_ilc/flc/tb-desy/2025/TB2025-03/beamData/Run_Data

for dir in "$Data_DIR"/eudaq_run_*; do
    if [ -d "$dir" ]; then
        settings_file="$dir/Run_Settings.txt"
        if [ -f "$settings_file" ]; then
            echo "Processing: $settings_file"
            grep "ThresholdDAC:" "$settings_file"
        fi
    fi
done