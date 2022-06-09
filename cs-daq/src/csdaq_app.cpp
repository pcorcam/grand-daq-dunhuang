#include <csdaq_app.h>
#include <utils.h>
#include <functional>
#include <cs_sys_config.h>
#include <message_impl.h>

using namespace std::placeholders;

using namespace grand;

CSDAQApp::CSDAQApp() {

}

void CSDAQApp::sysInit() {
    std::string defaultConfig = ::getenv("GRAND_DAQ_CONFIG");
    std::string defaultConfigSys = defaultConfig + "/sysconfig.yaml";

    CSSysConfig *m_sysConfig = CSSysConfig::instance();
    m_sysConfig->load(defaultConfigSys);

    m_client = new ZMQClient;
    APPConfig &appCfg = m_sysConfig->appConfig();

    m_client->setup(appCfg.networkInputBufferSize, appCfg.zmqSndBufferSize);
    m_msgDispatcher = new MessageDispatcher;

    m_t3Trigger = new T3Trigger;
    m_fh = new ESFileHeaderWriter;
    m_eventStore = new EventStore("GRAND", "TEST", "", 0, m_fh);

    m_msgDispatcher->addProcessor((MessageType)MT_T2, std::bind(&T3Trigger::processData, m_t3Trigger, _1, _2, _3));
    m_msgDispatcher->addProcessor((MessageType)MT_DAQEVENT, std::bind(&EventStore::processData, m_eventStore, _1, _2, _3));

    m_client->addCallback([this](std::string duID, char* data, size_t sz)->void {
            this->m_msgDispatcher->dispatch(duID, data, sz);
    });

    for(auto &duInfo: m_sysConfig->duConfigs()) {
        CLOG(INFO, "network") << "add server: " << duInfo.ID << ", " << duInfo.ip << ":" << duInfo.port;
        m_client->addClient(duInfo.ID, duInfo.ip, duInfo.port);
    }

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
    stopDAQ();
    LOG(INFO) << "starting DAQ...";
    initialize();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    configure(nullptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void CSDAQApp::stopDAQ() {
    // TODO: create command message and send
    LOG(INFO) << "stopping DAQ...";
    stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    terminate();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

bool CSDAQApp::initialize() {
    char buf[1024];
    CommandMessage msg(buf, 1024, true);
    msg.setCmd("INIT");
    m_client->writeAll(buf, msg.size());
    return true;
}

bool CSDAQApp::configure(void *param) {
    std::string defaultConfig = ::getenv("GRAND_DAQ_CONFIG");
    std::string defaultConfigAddr = defaultConfig + "/DU-address-map.yaml";
    std::string defaultConfigData = defaultConfig + "/DU-readable-conf.yaml";

    ElecConfig::instance()->load(defaultConfigAddr, defaultConfigData);

    char buf[100*1024];
    char elecParameter[100*1024];
    size_t elecParameterSize = ElecConfig::instance()->toShadowlist((uint8_t*)elecParameter);

    CommandMessage msg(buf, 100*1024, true);
    msg.setCmd("CONF", elecParameter, elecParameterSize);
    m_client->writeAll(buf, msg.size());
    return true;
}

bool CSDAQApp::start() {
    m_eventStore->openStream();

    char buf[1024];
    CommandMessage msg(buf, 1024, true);
    msg.setCmd("STAR");
    m_client->writeAll(buf, msg.size());
    return true;
}

bool CSDAQApp::stop() {
    char buf[1024];
    CommandMessage msg(buf, 1024, true);
    msg.setCmd("STOP");
    m_client->writeAll(buf, msg.size());

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    m_eventStore->closeStream();
    return true;
}

bool CSDAQApp::terminate() {
    char buf[1024];
    CommandMessage msg(buf, 1024, true);
    msg.setCmd("TERM");
    m_client->writeAll(buf, msg.size());
    return true;
}

bool CSDAQApp::toError() {
    return true;
}

