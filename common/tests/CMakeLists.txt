
module_test(TARGET test_buffer_pool
            SRCS test_buffer_pool.cpp
            LINKS grandcommon)

module_test(TARGET test_message
            SRCS test_message.cpp
            LINKS grandcommon)

module_test(TARGET test_client
            SRCS test_client.cpp
            LINKS csdaq)

module_test(TARGET test_server
            SRCS test_server.cpp
            LINKS csdaq)

module_test(TARGET test_communication
            SRCS test_communication.cpp
            LINKS csdaq)

module_test(TARGET test_send_recv_cmd
            SRCS test_send_recv_cmd.cpp
            LINKS csdaq)
