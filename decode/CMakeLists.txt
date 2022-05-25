cmake_minimum_required(VERSION 3.5)

project(decode_V1)

include_directories(/home/grand/workarea/grand-daq/decode/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(decode_v1_0 decode_v1_0.cpp)
