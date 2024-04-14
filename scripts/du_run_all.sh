#!/bin/sh

for i in $ALL_DUS; do
    sshpass -p root ssh root@$i /home/root/grand-daq/arm/grand-daq/scripts/du_run.sh
done
