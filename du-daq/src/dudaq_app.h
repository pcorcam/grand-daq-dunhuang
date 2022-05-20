#pragma once 

#include <du_sys_config.h>
#include <zmq_server.h>
#include <message_dispatcher.h>
#include <frontend.h>
#include <data_manager.h>

#include <map>

namespace grand {

class DUDAQApp {
public:
    DUDAQApp();
    void sysInit();
    void sysTerm();

private:
    DUSysConfig *m_sysConfig;
    MessageDispatcher *m_msgDispatcher;
    ZMQServer *m_server;
    IFrontend *m_frontend;
    DataManager *m_dataManager;

private:
    bool initialize();
    bool configure(void *param);
    bool start();
    bool stop();
    bool terminate();
    bool toError();

    void processCommand(char *data, size_t sz);
};

}
