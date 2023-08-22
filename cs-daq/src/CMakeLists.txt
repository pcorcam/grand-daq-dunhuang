
add_library(csdaq SHARED
    data_format.cpp
    cs_sys_config.cpp
    elec_config.cpp
    t3_trigger.cpp
    event_store.cpp
    raw_event_store.cpp
    status_processor.cpp
    csdaq_app.cpp
)

IF(NOT REAL_DU)
    install(TARGETS csdaq LIBRARY DESTINATION lib)
ENDIF(NOT REAL_DU)

