
add_executable(csdaq_run csdaq_run.cpp)
target_link_libraries(csdaq_run csdaq grandcommon)

IF(NOT REAL_DU)
    install(TARGETS csdaq_run RUNTIME DESTINATION .)
ENDIF(NOT REAL_DU)
