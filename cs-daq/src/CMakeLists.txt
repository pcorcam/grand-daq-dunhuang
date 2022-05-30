
add_library(csdaq SHARED
    cs_sys_config.cpp
    elec_config.cpp
    t3_trigger.cpp
    event_store.cpp
    status_processor.cpp
    csdaq_app.cpp
)

IF(NOT REAL_DU)
    install(TARGETS csdaq LIBRARY DESTINATION .)
ENDIF(NOT REAL_DU)

