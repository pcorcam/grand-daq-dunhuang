## execute this shell to run du-daq in all DUs
### duanbh 20250413

killall dudaq_run
source /home/root/grand-daq/arm/env.sh
nohup dudaq_run > /dev/null 2>&1 &
