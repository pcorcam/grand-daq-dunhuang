#!/bin/bash

run_time=$1
whole_run_time=172800
whole_realtime=$(date +%s)
whole_run_stop_time=${whole_realtime}

while [ true ];do

    whole_realtime=$(date +%s)
	if [ $((whole_realtime - whole_run_stop_time)) -ge $whole_run_time ];then
		echo "This run job is over!"
		break
	fi

    ssh root@192.168.61.35 "sh ifreboot.sh"
    ssh root@192.168.61.35 "source /home/root/grand-daq/arm/env.sh; nohup dudaq_run > null &"
    source /home/grand/workarea/grand-daq/env.sh
    filename=$(date "+%Y%m%d%H%M%S")
    nohup csdaq_run $run_time > /home/grand/workarea/grand-daq/logs/log_20hz_mode_${filename}_DU35_20dB_320M.txt 2>&1 &
    
    sleep 7201
    
    ssh root@192.168.61.35 "shutdown -r now"
    
    sleep 180
done
