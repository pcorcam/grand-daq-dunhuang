#!/bin/sh

CMAKE=cmake

SOFT_DIR=$(dirname $0)

echo $1
if [ "$1" == "arm" ]; then
    echo "building for ARM..."
    CXX_COMPILER=aarch64-linux-gnu-g++
    C_COMPILER=aarch64-linux-gnu-gcc
    TAG=arm
    REAL_DU=ON
else 
    echo "building for PC..."
    CXX_COMPILER=g++
    C_COMPILER=gcc
    TAG=x86_64
    REAL_DU=OFF
fi

build_dir="${SOFT_DIR}/build-${TAG}"

${CMAKE} -DCMAKE_CXX_COMPILER=${CXX_COMPILER} \
        -DCMAKE_C_COMPILER=${C_COMPILER} \
        -DCMAKE_INSTALL_PREFIX=${SOFT_DIR}/installed/${TAG} \
        -DUNIT_TEST=ON \
        -DREAL_DU=${REAL_DU} \
        -S . -B${build_dir}

cd ${build_dir}
#make clean
VERBOSE=1 make -j8 && make install

cd -
