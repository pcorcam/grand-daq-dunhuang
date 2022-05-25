export GRAND_DAQ_HOME=/home/grand/workarea/grand-daq.format
export GRAND_DAQ_CONFIG=${GRAND_DAQ_HOME}/cfgs
export GRAND_DAQ_CONFIG_LOG=${GRAND_DAQ_CONFIG}/logging

export GRAND_DATA_DIR=/home/grand/workarea/data
export GRAND_MAX_FILE_SIZE=500000000 # 500MB per file

export GRAND_EXTERNAL_HOME=/home/grand/pc_installed

export LD_LIBRARY_PATH=${GRAND_DAQ_HOME}/installed/x86_64/lib:${GRAND_EXTERNAL_HOME}/lib64:${GRAND_EXTERNAL_HOME}/lib:${LD_LIBRARY_PATH}
export PATH=${GRAND_DAQ_HOME}/installed/x86_64/bin:${PATH}
