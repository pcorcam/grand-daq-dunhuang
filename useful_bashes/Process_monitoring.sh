#!/bin/bash
while [ 1 ] ; do
sleep 3
    #if [ $(ps -ef|grep exe_name|grep -v grep|wc -l) -eq 0 ] ; then # 将exe_name替换成你想要监测的可执行程序名字
    #    sleep 1;
    #    echo "[`date +%F\ %T`] exe_name is offline, try to restart..." >> ./logs/check_es.log;
    #    ./exe_name &  # 将exe_name替换成你想要监测的可执行程序名字
    #else
    #    echo "[`date +%F\ %T`] exe_name is online..." >> ./logs/check_es.log;
    #fi
done

