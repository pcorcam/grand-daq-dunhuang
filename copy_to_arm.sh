#!/bin/sh

dir=$GRAND_DAQ_HOME/installed/arm
arm_dir=/home/root/grand-daq/arm/grand-daq

targets="192.168.61.24 192.168.61.53"
#targets="192.168.61.24"

for i in $targets; do
  if [ "$1" == "all" ]; then
      sshpass -p root scp -r $GRAND_DAQ_HOME/cfgs root@${i}:${arm_dir}/
      sshpass -p root scp -r $dir/lib/libgrandcommon.so root@${i}:${arm_dir}/lib/
  fi
  
  sshpass -p root scp -r $dir/lib/libdudaq.so root@${i}:${arm_dir}/lib/
  sshpass -p root scp -r $dir/bin/dudaq_run root@${i}:${arm_dir}/bin/
done
