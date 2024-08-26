#!/bin/bash

## Auther Duan Bohao
## Time 2023/08/23 19:33
## Auto shell to run csdaq_run

whole_realtime=$(date +%s)
whole_run_stop_time=${whole_realtime}
whole_run_time=864000
curr_run_time=$(date +%s)
last_run_time=${curr_run_time}
run_time=$1

while [ ture ];do
	whole_realtime=$(date +%s)
	if [ $((whole_realtime - whole_run_stop_time)) -ge $whole_run_time ];then
		echo "This run job is over!"
		break
	fi

	filename=$(date "+%Y%m%d%H%M%S")

	for vgaGain in -10 -5 0 5 10 15 20
	do
		whole_realtime=$(date +%s)
		if [ $((whole_realtime - whole_run_stop_time)) -ge $whole_run_time ];then
			echo "This run job is over!"
			break
		fi
		echo "now vga gain is $vgaGain dB"
		echo "in this gain, the job will run $run_time s"
		./a.out $vgaGain
		source /home/grand/workarea/grand-daq/env.sh
		nohup csdaq_run $run_time > /home/grand/workarea/grand-daq/logs/log_10s_mode_${filename}_8DUs_${vgaGain}dB_50M.txt 2>&1 &
		while [ true ];do
			curr_run_time=$(date +%s)
			# echo $curr_run_time
			if [  $((curr_run_time - last_run_time)) -ge $run_time ];then
				sleep 5 # time interval between each run
				last_run_time=$(date +%s)
				echo "last run time is  $last_run_time"
				break
			fi
			sleep 5
		done
	done
done

