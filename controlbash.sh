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
## GP65 TOTAL SSH OK, 20250427
# for du in 9 10 12 13 14 16 17 19 20 23 24 29 31 32 33 35 37 38 40 43 45 46 47 48 49 52 56 58 65 66 71 72 73 75 77 78 79 81 82 83 84 89 90 91 93 94
# for du in 9 10 11 12 13 14 16 17 18 19 20 22 23 24 29 3 30 32 33 34 35 37 38 39 40 42 43 44 45 46 47 48 49 51 52 54 55 56 58 65 66 71 72 73 74 75 76 77 78 81 82 83 84 85 86 88 89 9 90 91 92 93 94

## GP65 20250827
#for du in 3 9 10 11 12 13 14 16 17 18 19 20 22 23 24 29 30 32 33 34 35 37 38 39 40 41 42 43 44 45 46 47 48 49 51 52 54 55 56 57 58 59 65 66 71 72 73 74 75 76 77 78 81 82 83 84 85 86 88 89 90 91 92 93 94
# for du in 9 10 11 12 13
for du in 45 47 48 52 88
# for du in 14 16 17 18 19 20 22 23 24 29 30 32 33 34 35 37 38 39 40 41 42 43 44 45 46 47 48 49 51 52 54 55 56 57 58 59 65 66 71 72 73 74 75 76 77 78 81 82 83 84 85 86 88 89 90 91 92 93
do
	echo "du$du"; scp -i /home/grand/.ssh/id_rsa-nopass $para root@192.168.61.$du:$para2
done
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

