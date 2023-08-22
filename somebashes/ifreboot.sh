mkdir -p /run/grand-daq
cd /home/root/grand-daq
tar xzvf /home/root/grand-daq/arm.tar.gz 
cp -r /home/root/grand-daq/arm /run/grand-daq/
cd /run/grand-daq
source /run/grand-daq/arm/env.sh
