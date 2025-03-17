#!/bin/bash
# to reboot the computer in a fixed time

run_time=$1
whole_run_time=2592000
whole_realtime=$(date +%s)
whole_run_stop_time=${whole_realtime}

target_hour=22
target_minute=30 

current_hour=$(date +%H)
current_minute=$(date +%m)

current_time_in_minutes=$((10#$current_hour * 60 + 10#$current_minute))
target_time_in_minutes=$((10#$target_hour * 60 + 10#$target_minute))

while [ true ];do

    whole_realtime=$(date +%s)
    current_hour=$(date +%H)
    current_minute=$(date +%m)
    current_time_in_minutes=$((10#$current_hour * 60 + 10#$current_minute))
    
    if [ $((whole_realtime - whole_run_stop_time)) -ge $whole_run_time ];then
		echo "[NEWS]: This run job is over!"
		break
	fi

    for du in 13 17 19 20 29 31 32 41 71 75
    do 
        ssh root@192.168.61.$i "killall dudaq_run"
        ssh root@192.168.61.$i "sh ifreboot.sh"
        ssh root@192.168.61.$i "source /home/root/grand-daq/arm/env.sh; nohup dudaq_run > /dev/null 2>&1 &"
    done
    
    source /home/grand/workarea/grand-daq/env.sh
    filename=$(date "+%Y%m%d%H%M%S")
    mv /home/grand/workarea/grand-daq/cfgs/DU-readable-conf.yaml_11DUs_L1 /home/grand/workarea/grand-daq/cfgs/DU-readable-conf.yaml
    mv /home/grand/workarea/grand-daq/cfgs/sysconfig.yaml_11DUs_L1 /home/grand/workarea/grand-daq/cfgs/sysconfig.yaml
    nohup csdaq_run $run_time > /home/grand/workarea/grand-daq/logs/log_L1_mode_${filename}_11DUs_35M.txt 2>&1 &
    sleep 1
    mv /home/grand/workarea/grand-daq/cfgs/DU-readable-conf.yaml /home/grand/workarea/grand-daq/cfgs/DU-readable-conf.yaml_11DUs_L1
    mv /home/grand/workarea/grand-daq/cfgs/sysconfig.yaml /home/grand/workarea/grand-daq/cfgs/sysconfig.yaml_11DUs_L1 
    sleep $run_time
    
    if [ $current_time_in_minutes -gt $target_time_in_minutes ]; then      
		echo "[NEWS]: I will reboot all DUs included in this job!!!"
        for du in 13 17 19 20 29 31 32 41 71 75
        do
            ssh root@192.168.61.$i "sh BRD_rst.sh"
        done
	fi
    
    sleep 180
done
