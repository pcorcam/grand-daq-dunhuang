#!/bin/bash
#  Created by duanbh 2022/10/26

APP_HOME=/run/grand-daq

cmd=$1

if [ $cmd == "extract" ];then
ssh root@192.168.61.38 "${APP_HOME}/do.sh"
fi

#if [ $cmd == "source" ];then
#ssh root@192.168.61.38 "source ${APP_HOME}/arm/env.sh"
#fi

if [ $cmd == "dudaq_run" ];then
ssh root@192.168.61.38 "source ${APP_HOME}/arm/env.sh; dudaq_run" > /dev/null &
fi

if [ $cmd == "kill" ];then
ssh root@192.168.61.38 << eof
PID=ps -ef|grep "dudaq_run"|grep -v grep|awk '{print "$2"}'
echo PID
eof
#ssh root@192.168.61.38 "kill ${PID}"
fi
