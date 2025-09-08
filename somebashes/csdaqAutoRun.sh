#!/bin/bash
# to reboot the CSDAQ in a fixed time

run_time=$1
whole_run_time=2592000
whole_realtime=$(date +%s)
whole_run_stop_time=${whole_realtime}

target_hour=00
target_minute=15

current_hour=$(date +%H)
current_minute=$(date +%m)

current_time_in_minutes=$((10#$current_hour * 60 + 10#$current_minute))
target_time_in_minutes=$((10#$target_hour * 60 + 10#$target_minute))

while [ true ];do

        whole_realtime=$(date +%s)
        current_hour=$(date +%H)
        current_minute=$(date +%m)

    ## convert the time into seconds
    current_time_in_minutes=$((10#$current_hour * 60 + 10#$current_minute))

    if [ $((whole_realtime - whole_run_stop_time)) -ge $whole_run_time ];then
            echo "[NEWS]: This run job is over!"
            break
    fi

    # if [ $current_time_in_minutes -gt $target_time_in_minutes ]; then
    source /home/grand/workarea/grand-daq/env.sh
    filename=$(date "+%Y%m%d%H%M%S")
    mv /home/grand/workarea/grand-daq/cfgs/DU-readable-conf.yaml_11DUs_L1 /home/grand/workarea/grand-daq/cfgs/DU-readable-conf.yaml
    mv /home/grand/workarea/grand-daq/cfgs/sysconfig.yaml_11DUs_L1 /home/grand/workarea/grand-daq/cfgs/sysconfig.yaml
    nohup csdaq_run ${run_time} > /home/grand/workarea/grand-daq/logs/log_L1_mode_${filename}_11DUs_35M.txt 2>&1 &
    sleep 1
    mv /home/grand/workarea/grand-daq/cfgs/DU-readable-conf.yaml /home/grand/workarea/grand-daq/cfgs/DU-readable-conf.yaml_11DUs_L1
    mv /home/grand/workarea/grand-daq/cfgs/sysconfig.yaml /home/grand/workarea/grand-daq/cfgs/sysconfig.yaml_11DUs_L1
    echo "run time is ${run_time}"
    sleep ${run_time}
    # fi

    sleep 15
done
