#!/bin/sh

SOFT_DIR=$(dirname $0)

if [ "$1" == "arm" ]; then
    CXX_COMPILER=aarch64-linux-gnu-g++
    C_COMPILER=aarch64-linux-gnu-gcc
    TAG=arm
else 
    CXX_COMPILER=g++
    C_COMPILER=gcc
    TAG=x86_64
fi

build_dir="${SOFT_DIR}/build-${TAG}"

cmake3 -DCMAKE_CXX_COMPILER=${CXX_COMPILER} \
        -DCMAKE_C_COMPILER=${C_COMPILER} \
        -DCMAKE_INSTALL_PREFIX=${SOFT_DIR}/installed/${TAG} \
        -DUNIT_TEST=ON -S . -B${build_dir}

cd ${build_dir}
make -j4
make install

cd -
