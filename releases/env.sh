export GRAND_DAQ_HOME=<DAQ_INSTALLED_DIR>
export GRAND_DAQ_CONFIG=${GRAND_DAQ_HOME}/grand-daq/cfgs
export GRAND_DAQ_CONFIG_LOG=${GRAND_DAQ_CONFIG}/logging
export LD_LIBRARY_PATH=${GRAND_DAQ_HOME}/external/lib:${GRAND_DAQ_HOME}/external/lib64:${GRAND_DAQ_HOME}/grand-daq/lib:${LD_LIBRARY_PATH}
export PATH=${GRAND_DAQ_HOME}/grand-daq/bin:${PATH}

export GRAND_DATA_DIR=/home/wang/workarea/data
export GRAND_MAX_FILE_SIZE=500000000 # 500MB per file
