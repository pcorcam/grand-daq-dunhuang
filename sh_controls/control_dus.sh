#  !/bin/bash
#  Created by duanbh, 20221026

SUB_CONTROL_BASH_DIRECTORY=/home/grand/workarea/grand-daq/sh_controls/sh_subcontrols

cmd=$1
para=$2

# paras are: extract, dudaq_run
if [ $cmd == "all" ];then
sh ${SUB_CONTROL_BASH_DIRECTORY}/control_38.sh $para
sh ${SUB_CONTROL_BASH_DIRECTORY}/control_39.sh $para
sh ${SUB_CONTROL_BASH_DIRECTORY}/control_76.sh $para
sh ${SUB_CONTROL_BASH_DIRECTORY}/control_77.sh $para
sh ${SUB_CONTROL_BASH_DIRECTORY}/control_78.sh $para
sh ${SUB_CONTROL_BASH_DIRECTORY}/control_79.sh $para
sh ${SUB_CONTROL_BASH_DIRECTORY}/control_80.sh $para
sh ${SUB_CONTROL_BASH_DIRECTORY}/control_82.sh $para
sh ${SUB_CONTROL_BASH_DIRECTORY}/control_85.sh $para
sh ${SUB_CONTROL_BASH_DIRECTORY}/control_86.sh $para
sh ${SUB_CONTROL_BASH_DIRECTORY}/control_87.sh $para
sh ${SUB_CONTROL_BASH_DIRECTORY}/control_94.sh $para
fi
