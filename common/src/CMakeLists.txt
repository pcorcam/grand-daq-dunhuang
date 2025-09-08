IF(REAL_DU)
    add_library(grandcommon SHARED
        internal/static_init.cpp
        internal/buffer_pool.cpp
        internal/easylogging++.cc
        message.cpp
        message_impl.cpp
        server.cpp
        zmq_server.cpp
        message_dispatcher.cpp
    )
ELSE()
    add_library(grandcommon SHARED
        internal/static_init.cpp
        internal/buffer_pool.cpp
        internal/easylogging++.cc
        message.cpp
        message_impl.cpp
        client.cpp
        zmq_client.cpp
        server.cpp
        zmq_server.cpp
        message_dispatcher.cpp
        cs_sys_config.cpp
    )
ENDIF(REAL_DU)

#add_library(grandcommon SHARED
#        internal/static_init.cpp
#        internal/buffer_pool.cpp
#        internal/easylogging++.cc
#        message.cpp
#        message_impl.cpp
#	#client.cpp
#	#zmq_client.cpp
#        server.cpp
#        zmq_server.cpp
#        message_dispatcher.cpp
#	#cs_sys_config.cpp
#)

install(TARGETS grandcommon LIBRARY DESTINATION lib)
