
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
)
