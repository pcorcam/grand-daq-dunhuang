#pragma once 

#include <sys_config.h>
#include <zmq_server.h>
#include <message_dispatcher.h>

namespace grand {

class DUDAQApp {
public:
    DUDAQApp();
    void sysInit();

private:
    SysConfig *m_sysConfig;
    ZMQServer *m_server;
    MessageDispatcher *m_msgDispatcher;
};

}
