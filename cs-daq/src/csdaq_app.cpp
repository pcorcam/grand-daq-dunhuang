#include "csdaq_app.h"
#include <utils.h>
#include <functional>
#include <cs_sys_config.h>
#include <message_impl.h>

using namespace std::placeholders;
using namespace grand;

CSDAQApp::CSDAQApp() {

}

void CSDAQApp::sysInit() {
    // std::cout << "CSDAQApp::sysInit" << std::endl;
    std::string defaultConfig = ::getenv("GRAND_DAQ_CONFIG");
    std::string defaultConfigSys = defaultConfig + "/sysconfig.yaml";

    CSSysConfig *m_sysConfig = CSSysConfig::instance();
    m_sysConfig->load(defaultConfigSys);
    
    m_client = new ZMQClient;
    APPConfig &appCfg = m_sysConfig->appConfig();

    m_client->setup(appCfg.networkInputBufferSize, appCfg.zmqSndBufferSize);
    m_msgDispatcher = new MessageDispatcher;

    m_t3Trigger = new T3Trigger(m_cbTriggerDone, m_client);
    m_fh = new ESFileHeaderWriter;
    m_fh2 = new RawESFileHeaderWriter;

    m_daqMode = CSSysConfig::instance()->appConfig().daqMode;
    m_msgDispatcher->addProcessor((MessageType)MT_T2, std::bind(&T3Trigger::processData, m_t3Trigger, _1, _2, _3));
    if(m_daqMode == 2 || m_daqMode == 3) {
        m_dataFileName = CSSysConfig::instance()->appConfig().l3FileName;
        m_eventStore = new EventStore("GP80", m_dataFileName, "", 0, m_fh);
        m_msgDispatcher->addProcessor((MessageType)MT_DAQEVENT, std::bind(&EventStore::processData, m_eventStore, _1, _2, _3));
    }
    if(m_daqMode == 1 || m_daqMode == 3 || m_daqMode == 4) {
        m_dataFileName = CSSysConfig::instance()->appConfig().l1FileName;
        m_rawEventStore = new rawEventStore("GP80", m_dataFileName, "", 0, m_fh2, m_daqMode);
        m_msgDispatcher->addProcessor((MessageType)MT_RAWEVENT, std::bind(&rawEventStore::processData, m_rawEventStore, _1, _2, _3));
    }
    
    m_client->addCallback([this](std::string duID, char* data, size_t sz)->void {
        this->m_msgDispatcher->dispatch(duID, data, sz);
    }); // This func is tied by Client::addCallback->m_callbacks->Client::inputThread()->Client::initialize()


    for(auto &duInfo: m_sysConfig->duConfigs()) {
        CLOG(INFO, "network") << "add server: " << duInfo.ID << ", " << duInfo.ip << ":" << duInfo.port;
        m_client->addClient(duInfo.ID, duInfo.ip, duInfo.port);
    }

    if ( CSSysConfig::instance()->appConfig().t3TriggerType==0 && CSSysConfig::instance()->appConfig().dudaqFileInjection)
        stop_workaround = true;
    else
        stop_workaround = false;

    m_client->initialize();
    // std::cout << "CSDAQApp::sysInit end" << std::endl;
}

void CSDAQApp::sysTerm() {
    // std::cout << "sysTerm" << std::endl;
    m_client->terminate();

    delete m_client;
    delete m_msgDispatcher;
    delete m_t3Trigger;
    if(m_daqMode == 2 || m_daqMode == 3) {
        delete m_eventStore;
        m_eventStore = nullptr;
    }  
    
    delete m_fh;
    delete m_fh2;
    m_fh = nullptr;
    m_fh2 = nullptr;
    m_client = nullptr;
    m_msgDispatcher = nullptr;
    m_t3Trigger = nullptr;
    // std::cout << "sysTerm end" << std::endl;
}

void CSDAQApp::startDAQ() {
    // std::cout << "startDAQ" << std::endl;
    stopDAQ(true);
    LOG(INFO) << "starting DAQ...";
    initialize();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    configureOne(nullptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // std::cout << "startDAQ end" << std::endl;
}

void CSDAQApp::stopDAQ(bool init) {
    // std::cout << "stop DAQ" << std::endl;
    LOG(INFO) << "stopping DAQ...";
    // cout << "stopping" << endl;
    stop(init);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    terminate();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // std::cout << "stop DAQ end" << std::endl;
}

bool CSDAQApp::initialize() {
    char buf[1024];
    CommandMessage msg(buf, 1024, true);
    msg.setCmd("INIT");
    m_client->writeAll(buf, msg.size());
    return true;
}

bool CSDAQApp::configureOne(void *param) {
    std::string defaultConfig = ::getenv("GRAND_DAQ_CONFIG");
    std::string defaultSysConfigAddr = defaultConfig + "/sysconfig.yaml";
    std::string defaultConfigAddr = defaultConfig + "/DU-address-map.yaml";
    std::string defaultConfigData = defaultConfig + "/DU-readable-conf.yaml";

    ElecConfig::instance()->load(defaultConfigAddr, defaultConfigData);

    char buf[100*1024];
    char elecParameter[100*1024];

    CSSysConfig *m_sysConfig = CSSysConfig::instance();
    int i = 0;
    for(auto &duInfo: m_sysConfig->duConfigs()) {
        memset(elecParameter, 0, 100*1024);
        memset(buf, 0, 100*1024);
        size_t elecParameterSize = ElecConfig::instance()->toShadowlist((uint8_t*)elecParameter, (char*)duInfo.ID.c_str());
        CommandMessage msg(buf, 100*1024, true);
        msg.setCmd("CONF", elecParameter, elecParameterSize);
        m_client->writeOne( duInfo.ID, buf, msg.size());
    }
    return true;
}

bool CSDAQApp::start() {
    // std::cout << "CSDAQApp::start" << std::endl;
    if(m_daqMode == 2 || m_daqMode == 3) 
        m_eventStore->openStream();
    if(m_daqMode == 1 || m_daqMode == 3) 
        m_rawEventStore->openStream();
    if(m_daqMode == 4) {
        m_rawEventStore->openStream();
        m_rawEventStore->openMDStream();
    }
        
    char buf[1024];
    CommandMessage msg(buf, 1024, true);
    msg.setCmd("STAR");
    m_client->writeAll(buf, msg.size());
    // std::cout << "CSDAQApp::start end" << std::endl;
    return true;
}

bool CSDAQApp::stop(bool init) {
    CLOG(INFO, "app") << "Initiating DAQ shutdown...";

    // std::cout << "CSDAQApp::stop" << std::endl;
    // First notify clients to stop sending data
    char buf[1024];
    CommandMessage msg(buf, 1024, true);
    // By default set STOP here
    if (!stop_workaround)
        msg.setCmd("STOP");
    m_client->writeAll(buf, msg.size());
    // In buggy situtaions (file injection + old trigger + Lech's computer), STOP can't be added before writeAll
    if (stop_workaround)
        msg.setCmd("STOP");


    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    if(m_daqMode == 2 || m_daqMode == 3) {
        // Stop T3Trigger first - this will wait for all processing to complete
        CLOG(INFO, "app") << "Stopping T3Trigger...";
        m_t3Trigger->stop(init);  // This now waits for completion
        m_client->waitForWriteAllCompletion();
        if (!init) usleep(100000);
        // Now safe to close the event store
        CLOG(INFO, "app") << "Closing event store...";
        m_eventStore->closeStream();
    }

    if(m_daqMode == 1 || m_daqMode == 3){
        m_rawEventStore->closeStream();
    }

    if(m_daqMode == 4) {
        m_rawEventStore->closeStream();
        m_rawEventStore->closeMDStream();
    }
    // std::cout << "CSDAQApp::stop end" << std::endl;
    return true;
}

bool CSDAQApp::terminate() {
    // std::cout << "CSDAQApp::terminate" << std::endl;
    char buf[1024];
    memset(buf, 0, 1024);
    CommandMessage msg(buf, 1024, true);
    msg.setCmd("TERM");
    m_client->writeAll(buf, msg.size());
    // std::cout << "CSDAQApp::terminate end" << std::endl;
    return true;
}

bool CSDAQApp::toError() {
    return true;
}

