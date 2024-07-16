#!/bin/bash

# 设置TensorFlow目录
TFLITE_DIR=/home/grand/externals/tensorflow-master

# 设置交叉编译器
export CC=aarch64-linux-gnu-gcc
export CXX=aarch64-linux-gnu-g++

# 创建构建目录
mkdir -p $TFLITE_DIR/tflite_build
cd $TFLITE_DIR/tflite_build

# 配置并编译TensorFlow Lite
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=../tensorflow/lite/tools/make/toolchains/aarch64_linux_gcc.cmake \
  -DTFLITE_ENABLE_XNNPACK=OFF \
  -DCMAKE_BUILD_TYPE=Release

make -j$(nproc) tensorflow-lite