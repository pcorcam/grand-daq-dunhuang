#!/bin/bash
# to reboot the DU in a fixed time

whole_run_time=2592000
oneDayTime=86400
whole_startTime=$(date +%s)

while [ true ];do

    whole_realtime=$(date +%s)

    if [ $((whole_realtime - whole_startTime)) -ge $whole_run_time ]; then
		  echo "[NEWS]: This run job is over!"
		  break
	  fi

    sh /home/root/ifreboot.sh
    sleep $oneDayTime
    whole_realtime=$(date +%s)

    if [ $(whole_realtime - whole_startTime) -ge $oneDayTime ]; then      
	echo "[NEWS]: I will reboot all DUs included in this job!!!"
        killall dudaq_run
        # shutdown -r now
	sh /home/root/BRD_rst.sh
        sleep 180
        whole_startTime=$whole_realtime
	  fi

done
