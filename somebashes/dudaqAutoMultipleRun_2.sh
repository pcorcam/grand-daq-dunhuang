#!/bin/bash

run_time=$1
whole_run_time=2592000
whole_realtime=$(date +%s)
whole_run_stop_time=${whole_realtime}

while [ true ];do

    whole_realtime=$(date +%s)
	if [ $((whole_realtime - whole_run_stop_time)) -ge $whole_run_time ];then
		echo "This run job is over!"
		break
	fi

    ssh root@192.168.61.31 "sh ifreboot.sh"
    ssh root@192.168.61.31 "source /home/root/grand-daq/arm/env.sh; nohup dudaq_run > null &"

    # ssh root@192.168.61.75 "sh ifreboot.sh"
    # ssh root@192.168.61.75 "source /home/root/grand-daq/arm/env.sh; nohup dudaq_run > null &"
    
    # ssh root@192.168.61.76 "sh ifreboot.sh"
    # ssh root@192.168.61.76 "source /home/root/grand-daq/arm/env.sh; nohup dudaq_run > null &"
    
    source /home/grand/workarea/grand-daq/env.sh
    filename=$(date "+%Y%m%d%H%M%S")
    mv /home/grand/workarea/grand-daq/cfgs/sysconfig_2.yaml /home/grand/workarea/grand-daq/cfgs/sysconfig.yaml
    nohup csdaq_run $run_time > /home/grand/workarea/grand-daq/logs/log_20hz_mode_${filename}_DU31_30Day_620M.txt 2>&1 &
    sleep 11
    mv /home/grand/workarea/grand-daq/cfgs/sysconfig.yaml /home/grand/workarea/grand-daq/cfgs/sysconfig_2.yaml
    sleep $run_time

    ssh root@192.168.61.31 "shutdown -r now"
    # ssh root@192.168.61.75 "shutdown -r now"
    # ssh root@192.168.61.76 "shutdown -r now"
    
    sleep 190
done
