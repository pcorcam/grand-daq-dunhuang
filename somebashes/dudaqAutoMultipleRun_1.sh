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
    
    echo "new run"

    # ssh root@192.168.61.32 "sh ifreboot.sh"
    # ssh root@192.168.61.32 "source /home/root/grand-daq/arm/env.sh; nohup dudaq_run > null &"

    ssh root@192.168.61.85 "sh ifreboot.sh"
    ssh root@192.168.61.85 "source /home/root/grand-daq/arm/env.sh; nohup dudaq_run > null &"
    # ssh root@192.168.61.29 "source /run/grand-daq/arm/env.sh; nohup dudaq_run > null &"

    source /home/grand/workarea/grand-daq/env.sh
    filename=$(date "+%Y%m%d%H%M%S")
    mv /home/grand/workarea/grand-daq/cfgs/sysconfig_1.yaml /home/grand/workarea/grand-daq/cfgs/sysconfig.yaml
    nohup csdaq_run $run_time > /home/grand/workarea/grand-daq/logs/log_20hz_mode_${filename}_DU29_620M.txt 2>&1 &
    sleep 11
    mv /home/grand/workarea/grand-daq/cfgs/sysconfig.yaml /home/grand/workarea/grand-daq/cfgs/sysconfig_1.yaml
    sleep $run_time
    
    # ssh root@192.168.61.20 "shutdown -r now"
    # ssh root@192.168.61.21 "shutdown -r now"
    # ssh root@192.168.61.32 "shutdown -r now"
    ssh root@192.168.61.85 "shutdown -r now"
    
    sleep 190
done
