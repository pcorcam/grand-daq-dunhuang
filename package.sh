#!/bin/sh

EXTERNAL_DIR_PC=/home/grand/pc_installed
EXTERNAL_DIR_ARM=/home/grand/arm_a_installed

SOFT_DIR=$(dirname $0)

if [ "$1" == "arm" ]; then
    CXX_COMPILER=aarch64-linux-gnu-g++
    C_COMPILER=aarch64-linux-gnu-gcc
    TAG=arm
    EXTERNAL_DIR=${EXTERNAL_DIR_ARM}
else 
    CXX_COMPILER=g++
    C_COMPILER=gcc
    TAG=x86_64
    EXTERNAL_DIR=${EXTERNAL_DIR_PC}
fi

soft_installed="${SOFT_DIR}/installed/${TAG}"
external_installed=${EXTERNAL_DIR}

tmp_dir_prefix=/tmp/grand_tmp
tmp_sub=grand-daq-release
tmp_dir=${tmp_dir_prefix}/${tmp_sub}

rm -rf ${tmp_dir}

mkdir -p ${tmp_dir}/grand-daq
mkdir -p ${tmp_dir}/external

cp -r ${soft_installed}/* ${tmp_dir}/grand-daq/
cp -r ${external_installed}/* ${tmp_dir}/external/
cp -r ${SOFT_DIR}/releases/env.sh ${tmp_dir}/grand-daq/

rm -f ${SOFT_DIR}/releases/grand-daq-release.tar.gz
tar czf ${SOFT_DIR}/releases/grand-daq-release.tar.gz -C ${tmp_dir_prefix} ${tmp_sub}

echo "software is packaged in ${SOFT_DIR}/releases/grand-daq-release.tar.gz"
