#!/bin/sh

EXTERNAL_DIR_PC=/home/grand/pc_installed
EXTERNAL_DIR_ARM=/home/grand/arm_a_installed

EXTERNAL_LIB_PC="libyaml-cpp.so libyaml-cpp.so.0.7 libyaml-cpp.so.0.7.0 libzmq.so libzmq.so.5 libzmq.so.5.2.5"
EXTERNAL_LIB_ARM="libstdc++.so.6.0.24 libstdc++.so.6 libstdc++.so libgcc_s.so.1 libgcc_s.so libzmq.so libzmq.so.5 libzmq.so.5.2.5"

SOFT_DIR=$(dirname $0)

if [ "$1" == "arm" ]; then
    CXX_COMPILER=aarch64-linux-gnu-g++
    C_COMPILER=aarch64-linux-gnu-gcc
    TAG=arm
    EXTERNAL_DIR=${EXTERNAL_DIR_ARM}
    EXTERNAL_LIB=${EXTERNAL_LIB_ARM}
else 
    CXX_COMPILER=g++
    C_COMPILER=gcc
    TAG=x86_64
    EXTERNAL_DIR=${EXTERNAL_DIR_PC}
    EXTERNAL_LIB=${EXTERNAL_LIB_PC}
fi

ENV_LIB=env-${TAG}.sh

soft_installed="${SOFT_DIR}/installed/${TAG}"
external_installed=${EXTERNAL_DIR}

tmp_dir_prefix=/tmp/grand_tmp
tmp_sub=${TAG}
tmp_dir=${tmp_dir_prefix}/${tmp_sub}

rm -rf ${tmp_dir}

mkdir -p ${tmp_dir}/grand-daq
mkdir -p ${tmp_dir}/external

cp -r ${soft_installed}/* ${tmp_dir}/grand-daq/
mkdir -p ${tmp_dir}/external/lib
for i in ${EXTERNAL_LIB}; do 
    cp -r ${external_installed}/lib/$i ${tmp_dir}/external/lib/
done
cp -r ${external_installed}/bin ${tmp_dir}/external/
cp -r ${SOFT_DIR}/releases/env-${TAG}.sh ${tmp_dir}/env.sh

rm -f ${SOFT_DIR}/releases/${TAG}.tar.gz
tar czf ${SOFT_DIR}/releases/${TAG}.tar.gz -C ${tmp_dir_prefix} ${tmp_sub}

echo "software is packaged in ${SOFT_DIR}/releases/${TAG}.tar.gz"
