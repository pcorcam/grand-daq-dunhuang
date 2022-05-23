#!/bin/sh

dir=$GRAND_DAQ_HOME/installed/arm

if [ "$1" == "all" ]; then
    scp -r $GRAND_DAQ_HOME/cfgs $dir/lib/libgrandcommon.so root@192.168.10.2:~/grand-daq/
fi

scp -r $dir/lib/libdudaq.so $dir/bin/dudaq_run root@192.168.10.2:~/grand-daq/
