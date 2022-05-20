project(cs-daq)

cmake_minimum_required(VERSION 3.12)

include(macros.cmake)

OPTION(UNIT_TEST "enable/disable unit tests" OFF)
OPTION(REAL_DU "enable/disable compile for ARM" OFF)

IF(UNIT_TEST)
    ADD_DEFINITIONS(-DUNIT_TEST)
ENDIF(UNIT_TEST)

IF(REAL_DU)
    ADD_DEFINITIONS(-DREAL_DU)
ENDIF(REAL_DU)

IF(NOT DEFINED EXTERNAL_DIR)
    IF(REAL_DU)
        set(EXTERNAL_DIR /data/gv-dev/sys/grand/arm_installed)
    ELSE()
        set(EXTERNAL_DIR /data/gv-dev/sys/grand/pc_installed)
    ENDIF(REAL_DU)
ENDIF(NOT DEFINED EXTERNAL_DIR)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
enable_testing()

include_directories(${EXTERNAL_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common/src)

link_directories(${EXTERNAL_DIR}/lib ${EXTERNAL_DIR}/lib64)
link_libraries(pthread zmq)

set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -pipe -lstdc++")

ADD_DEFINITIONS(-DELPP_NO_LOG_TO_FILE -DELPP_NO_DEFAULT_LOG_FILE -DELPP_THREAD_SAFE)

add_subdirectory(common)
add_subdirectory(du-daq)

IF(NOT REAL_DU)
add_subdirectory(cs-daq)
add_subdirectory(tests)
ENDIF(NOT REAL_DU)
