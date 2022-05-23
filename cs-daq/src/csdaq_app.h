#pragma once 

#include <cs_sys_config.h>
#include <elec_config.h>
#include <zmq_client.h>
#include <message_dispatcher.h>
#include <t3_trigger.h>
#include <event_store.h>

namespace grand {

class CSDAQApp {
public:
    CSDAQApp();
    void sysInit();
    void sysTerm();

    void startDAQ();
    void stopDAQ();

private:
    CSSysConfig *m_sysConfig;
    ZMQClient *m_client;
    MessageDispatcher *m_msgDispatcher;

    T3Trigger *m_t3Trigger;
    ESFileHeaderWriter *m_fh;
    EventStore *m_eventStore;

    bool initialize();
    bool configure(void *param);
    bool start();
    bool stop();
    bool terminate();
    bool toError();
};

}
