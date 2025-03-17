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
echo "du32"; scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.32:$para2
echo "du77";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.77:$para2
echo "du49";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.49:$para2
echo "du79";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.79:$para2
echo "du52";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.52:$para2
echo "du44";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.44:$para2
echo "du9";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.9:$para2
echo "du13";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.13:$para2
echo "du81";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.81:$para2
echo "du33";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.33:$para2
echo "du93";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.93:$para2
echo "du46";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.46:$para2
echo "du92";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.92:$para2
echo "du84";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.84:$para2
echo "du82";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.82:$para2
echo "du3";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.3:$para2
echo "du20";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.20:$para2
echo "du75";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.75:$para2
echo "du29";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.29:$para2
echo "du74";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.74:$para2
echo "du78";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.78:$para2
echo "du85";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.85:$para2
echo "du19";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.19:$para2
echo "du94";scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.94:$para2
# scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.86:$para2
# scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.89:$para2
# scp $para root@192.168.61.94:$para2
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

