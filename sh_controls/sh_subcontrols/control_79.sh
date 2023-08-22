#!/bin/bash
#  Created by duanbh 2022/10/26

APP_HOME=/run/grand-daq

cmd=$1

if [ $cmd == "extract" ];then
ssh root@192.168.61.79 "${APP_HOME}/do.sh"
fi

if [ $cmd == "dudaq_run" ];then
ssh root@192.168.61.79 "source ${APP_HOME}/arm/env.sh; dudaq_run" > /dev/null &
fi
