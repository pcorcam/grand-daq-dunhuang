#include <csdaq_app.h>
#include <utils.h>
#include <functional>
#include <sys_config.h>

using namespace std::placeholders;

using namespace grand;

CSDAQApp::CSDAQApp() {

}

void CSDAQApp::sysInit() {
    std::string defaultConfig = ::getenv("GRAND_CSDAQ_CONFIG");
    std::string defaultConfigAddr = defaultConfig + "/DU-address-map.yaml";
    std::string defaultConfigData = defaultConfig + "/DU-readable-conf.yaml";

    SysConfig *m_sysConfig = SysConfig::instance();
    ElecConfig *m_elecConfig = ElecConfig::instance();
    m_elecConfig->load(defaultConfigAddr, defaultConfigData);

    m_client = new ZMQClient;
    APPConfig &appCfg = m_sysConfig->appConfig();

    m_client->setup(appCfg.networkInputBufferSize, appCfg.zmqSndBufferSize);
    m_msgDispatcher = new MessageDispatcher;

    m_t3Trigger = new T3Trigger;
    m_fh = new ESFileHeaderWriter;
    m_eventStore = new EventStore("GRAND", "TEST", "", 0, m_fh);

    m_msgDispatcher->addProcessor((MessageType)MT_T2, std::bind(&T3Trigger::processData, m_t3Trigger, _1, _2, _3));
    m_msgDispatcher->addProcessor((MessageType)MT_DAQEVENT, std::bind(&EventStore::processData, m_eventStore, _1, _2, _3));

    for(auto &duInfo: m_sysConfig->duConfigs()) {
        m_client->addClient(duInfo.ID, duInfo.ip, duInfo.port);
    }

    m_client->addCallback([this](std::string duID, char* data, size_t sz)->void { 
        (*this->m_msgDispatcher)(duID, data, sz); 
    });
    
    m_client->initialize();
}

void CSDAQApp::sysTerm() {
    m_client->terminate();

    delete m_client;
    delete m_msgDispatcher;
    delete m_t3Trigger;
    delete m_eventStore;
    
    m_client = nullptr;
    m_msgDispatcher = nullptr;
    m_t3Trigger = nullptr;
    m_eventStore = nullptr;
}

void CSDAQApp::startDAQ() {
    // TODO: create command message and send
    LOG(INFO) << "starting DAQ...";
    
}

void CSDAQApp::stopDAQ() {
    // TODO: create command message and send
    LOG(INFO) << "stopping DAQ...";
}
