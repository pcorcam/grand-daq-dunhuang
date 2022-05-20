project(wns_daq)

cmake_minimum_required(VERSION 3.12)

link_directories(${EXTERNAL_DIR}/lib ${EXTERNAL_DIR}/lib64)
include_directories(${EXTERNAL_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

link_libraries(pthread zmq)

add_subdirectory(src)
add_subdirectory(tests)

