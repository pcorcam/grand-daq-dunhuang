#!/bin/sh

cd /home/root/grand-daq/arm
source ./env.sh

pids=`ps -ef|grep dudaq_run|grep -v grep|awk '{print $1}'`
for i in $pids; do 
    kill $i
done

usleep 1000

(dudaq_run > /dev/null 2>&1 &)
