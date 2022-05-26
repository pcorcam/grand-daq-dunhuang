#!/bin/sh

dir=$GRAND_DAQ_HOME/installed/arm
arm_dir=/home/root/grand-daq/arm/grand-daq

if [ "$1" == "all" ]; then
    scp -r $GRAND_DAQ_HOME/cfgs root@192.168.10.2:${arm_dir}/
    scp -r $dir/lib/libgrandcommon.so root@192.168.10.2:${arm_dir}/lib/
fi

scp -r $dir/lib/libdudaq.so root@192.168.10.2:${arm_dir}/lib/
scp -r $dir/bin/dudaq_run root@192.168.10.2:${arm_dir}/bin/

