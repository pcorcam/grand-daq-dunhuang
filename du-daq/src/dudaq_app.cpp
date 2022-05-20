#include <csdaq_app.h>
#include <utils.h>
#include <functional>
#include <sys_config.h>

using namespace std::placeholders;
using namespace grand;

DUDAQApp::DUDAQApp() {

}

void DUDAQApp::sysInit() {
    SysConfig *m_sysConfig = SysConfig::instance();

    m_server = new ZMQServer;
    m_client->setup(m_sysConfig->inputBufferSize, m_sysConfig->backendBindUrl, m_sysConfig->maxClientAddressSize);
    m_msgDispatcher = new MessageDispatcher;

    m_msgDispatcher->addProcessor((MessageType)MT_T2, std::bind(&T3Trigger::processData, m_t3Trigger, _1, _2, _3));

    for(auto &duInfo: m_sysConfig->duConfigs()) {
        m_client->addClient(duInfo.ID, duInfo.ip, duInfo.port);
    }

    m_client->addCallback([this](std::string duID, char* data, size_t sz)->void { 
        (*this->m_msgDispatcher)(duID, data, sz); 
    });
    
    m_client->initialize();
}

