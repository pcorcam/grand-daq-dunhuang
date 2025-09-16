project(cs-daq)

cmake_minimum_required(VERSION 3.0)

include(macros.cmake)

OPTION(UNIT_TEST "enable/disable unit tests" OFF)
OPTION(REAL_DU "enable/disable compile for ARM" OFF)

IF(UNIT_TEST)
    ADD_DEFINITIONS(-DUNIT_TEST)
ENDIF(UNIT_TEST)

IF(REAL_DU)
    ADD_DEFINITIONS(-DREAL_DU)
ENDIF(REAL_DU)

set(EXTERNAL_DIR_ARM /home/grand/external/arm_installed)
set(EXTERNAL_DIR_PC /home/grand/external/pc_installed)
set(TFLITE_DIR /home/grand/external/tensorflow-2.17.0)

#set(EXTERNAL_DIR_ARM /data/gv-dev/sys/grand/arm_installed)
#set(EXTERNAL_DIR_PC /data/gv-dev/sys/grand/pc_installed)

IF(REAL_DU)
    set(EXTERNAL_DIR "${EXTERNAL_DIR_ARM}" CACHE PATH "external software install path")
    #set(TFLITE_DIR /home/grand/external/tensorflow-2.17.0)
ELSE()
    set(EXTERNAL_DIR "${EXTERNAL_DIR_PC}" CACHE PATH "external software install path")
ENDIF(REAL_DU)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS ON)

enable_testing()
include_directories(${EXTERNAL_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common/src)

link_directories(${EXTERNAL_DIR}/lib ${EXTERNAL_DIR}/lib64)


link_libraries(pthread zmq)

# IF(REAL_DU)
#     include_directories(/home/grand/external/tensorflow-2.17.0/tensorflow/lite)
#     include_directories(${TFLITE_DIR})
#     link_directories(${TFLITE_DIR})
#     link_libraries(tensorflowlite_c)
# ENDIF(REAL_DU)

include_directories(${TFLITE_DIR})
link_directories(${TFLITE_DIR})
link_libraries(tensorflowlite_c)

#############
# Include path to Eigen library for template L2 trigger
# P. Correa, 2024-11-05

include_directories(/usr/local/include)
#############

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O3 -pipe -lstdc++")

ADD_DEFINITIONS(-DELPP_NO_LOG_TO_FILE -DELPP_NO_DEFAULT_LOG_FILE -DELPP_THREAD_SAFE)

add_subdirectory(common)
add_subdirectory(du-daq)

IF(NOT REAL_DU)
    add_subdirectory(cs-daq)
    add_subdirectory(decode)
    # add_subdirectory(tests)
ENDIF(NOT REAL_DU)

install(DIRECTORY cfgs DESTINATION .)

