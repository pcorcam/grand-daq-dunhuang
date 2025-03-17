
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../du-daq/src ${CMAKE_CURRENT_SOURCE_DIR}/../cs-daq/src)
link_libraries(grandcommon)

module_test(TARGET test_statemachine_cscommand
            SRCS test_statemachine_cscommand.cpp
            LINKS csdaq dudaq)

