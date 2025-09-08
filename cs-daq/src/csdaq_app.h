#pragma once 

#include <cs_sys_config.h>
#include <elec_config.h>
#include <zmq_client.h>
#include <message_dispatcher.h>
#include "t3_trigger.h"
#include <event_store.h>
#include <raw_event_store.h>
#include "yaml-cpp/yaml.h"
#include "yaml-cpp/node/parse.h"

namespace grand {

class CSDAQApp {
public:
    CSDAQApp();
    void sysInit();
    void sysTerm();

    void startDAQ();
    void stopDAQ(bool init=true);

    void generateT3();

private:
    CSSysConfig *m_sysConfig;
    ZMQClient *m_client;
    MessageDispatcher *m_msgDispatcher;
    T3Trigger *m_t3Trigger;
    ESFileHeaderWriter *m_fh;
    RawESFileHeaderWriter *m_fh2;
    EventStore *m_eventStore;
    rawEventStore *m_rawEventStore;
    
    TriggerDone m_cbTriggerDone;
    
    YAML::Node m_config;
    std::vector<DUConfig> m_duInfo;

    std::thread *m_thread;
    bool initialize();
    bool configure(void *param);
    bool configureOne(void *param);
    bool getT3TriggerEvent();
    bool start();
    bool stop(bool init=true);
    bool terminate();
    bool toError();

    int m_daqMode = 0;
    std::string m_dataFileName;

    bool stop_workaround=false;
};

}
