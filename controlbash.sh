#!/bin/bash

BASHS_PATH=/home/grand/tests/duanbhtest/udp_test/udp_bashs
APP_HOME=/home/root/udp-test

cmd=$1
OBJ=$2
Type=$3
para=$3
para2=$4
para3=$5
para4=$6
para5=$7
#duID=$7
#duID2=$8
#duID3=$9
#duID4=${10}


if [ $cmd == "cp" -a $OBJ == "all" ];then
scp $para root@192.168.61.10:$para2
scp $para root@192.168.61.13:$para2
#scp $para root@192.168.61.16:$para2
scp $para root@192.168.61.17:$para2
scp $para root@192.168.61.19:$para2
scp $para root@192.168.61.20:$para2
scp $para root@192.168.61.21:$para2
#scp $para root@192.168.61.22:$para2
scp $para root@192.168.61.29:$para2
scp $para root@192.168.61.31:$para2
scp $para root@192.168.61.32:$para2
#scp $para root@192.168.61.33:$para2
scp $para root@192.168.61.35:$para2
scp $para root@192.168.61.41:$para2
scp $para root@192.168.61.75:$para2
scp $para root@192.168.61.76:$para2
scp $para root@192.168.61.85:$para2
#scp $para root@192.168.61.78:$para2
#scp $para root@192.168.61.79:$para2
#scp $para root@192.168.61.80:$para2
#scp $para root@192.168.61.81:$para2
#scp $para root@192.168.61.82:$para2
#scp $para root@192.168.61.83:$para2
#scp $para root@192.168.61.84:$para2
#scp $para root@192.168.61.85:$para2
#scp $para root@192.168.61.86:$para2
#scp $para root@192.168.61.87:$para2
#scp $para root@192.168.61.88:$para2
#scp $para root@192.168.61.94:$para2
fi

if [ $cmd == "cp" -a $OBJ == "one" ];then
scp $para root@192.168.61.$para3:$para2
fi

if [ $cmd == "cp" -a $OBJ == "two" ];then
scp $para root@192.168.61.$para2:$para4
scp $para root@192.168.61.$para3:$para4
fi

if [ $cmd == "cp" -a $OBJ == "two" -a $Type == "file" ];then
scp -r $para root@192.168.61.$para2:$para4
scp -r $para root@192.168.61.$para3:$para4
fi

if [ $cmd == "cp" -a $OBJ == "three" ];then
scp $para root@192.168.61.$para2:$para5
scp $para root@192.168.61.$para3:$para5
scp $para root@192.168.61.$para4:$para5
fi

#argv: all
#para: run time, para2: freq_min, para3: freq_max, para4: bin
if [ $cmd == "autorun" -a $OBJ == "all" ];then
sh ${BASHS_PATH}/udp_bash39.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash53.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash79.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash80.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash81.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash82.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash83.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash84.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash85.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash86.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash87.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash88.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash94.sh $cmd $para $para2 $para3 $para4
fi

#argv 1
#para: run time, para2: freq_min, para3: freq_max, para4: bin, duID
if [ $cmd == "autorun" -a $OBJ == "one" ];then
sh ${BASHS_PATH}/udp_bash$duID.sh $cmd $para $para2 $para3 $para4
fi

#argv 3
#para: run time, para2: freq_min, para3: freq_max, para4: bin, duID
if [ $cmd == "autorun" -a $OBJ == "three" ];then
sh ${BASHS_PATH}/udp_bash$duID.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash$duID2.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash$duID3.sh $cmd $para $para2 $para3 $para4
fi

#argv 4 
#para: run time, para2: freq_min, para3: freq_max, para4: bin, duID
if [ $cmd == "autorun" -a $OBJ == "four" ];then
sh ${BASHS_PATH}/udp_bash$duID.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash$duID2.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash$duID3.sh $cmd $para $para2 $para3 $para4
sh ${BASHS_PATH}/udp_bash$duID4.sh $cmd $para $para2 $para3 $para4
fi

