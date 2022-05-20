
module_test(TARGET test_elec_config
            SRCS test_elec_config.cpp
            LINKS csdaq)

module_test(TARGET test_sys_config
            SRCS test_sys_config.cpp
            LINKS csdaq)

module_test(TARGET test_event_store
            SRCS test_event_store.cpp
            LINKS csdaq)

module_test(TARGET test_csdaq_app
            SRCS test_csdaq_app.cpp
            LINKS csdaq)
