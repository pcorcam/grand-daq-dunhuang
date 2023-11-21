/************************/
// Created by gumh, duanbh
// 2022.09.02
/************************/

#include <dudaq_app.h>
#include <utils.h>
#include <functional>
#include <du_sys_config.h>
#include <du_fsm.h>
#include <scope_dummy.h>
#include <scope_a.h>
#include <message_impl.h>
#include <mutex>

using namespace std::placeholders;
using namespace grand;

int duID;

DUDAQApp::DUDAQApp() {
    
}

void DUDAQApp::sysInit() {
    DUSysConfig *m_sysConfig = DUSysConfig::instance();

#ifdef REAL_DU
    m_frontend = new ScopeA;
#else
    m_frontend = new ScopeDummy;
#endif
    
    m_server = new ZMQServer;
    // std::cout << "Now will connect scoket " << m_sysConfig->backendBindUrl << std::endl;
    m_server->setup(m_sysConfig->messageInputBufferSize, m_sysConfig->backendBindUrl, m_sysConfig->maxClientAddressSize);
    m_msgDispatcher = new MessageDispatcher;
    m_msgDispatcher->addProcessor((MessageType)MT_CMD, std::bind(&DUDAQApp::processCommand, this, _1, _2));
    
    m_server->setCallback([this](char* data, size_t sz)->void { 
        this->m_msgDispatcher->dispatch(data, sz); 
    });
    m_dataManager = new DataManager;
    
    m_dataManager->setEventOutput([this](char *data, size_t sz)->void {
        this->m_server->write(data, sz); // send data here.
    });
    
    m_dataManager->setT2EventOutput([this](char *data, size_t sz)->void {
        this->m_server->write(data, sz);
    });

    m_dataManager->setRawEventOutput([this](char *data, size_t sz)->void {
        this->m_server->write(data, sz);
    });
    
    m_dataManager->initialize();

    DUFSM::start();

    m_server->initialize();
}

void DUDAQApp::sysTerm() {
    m_frontend->terminate();
    m_server->terminate();
    m_dataManager->terminate();

    delete m_msgDispatcher;
    delete m_dataManager;
    delete m_server;
    delete m_frontend;
}

void DUDAQApp::processCommand(char *data, size_t sz) {
    CommandMessage msg(data, sz);
    std::string cmd = msg.cmd();

    if(cmd == "INIT") {
        EInitialize e;
        e.fun = std::bind(&DUDAQApp::initialize, this);
        DUFSM::sendEvent(e);
    }
    else if(cmd == "CONF") {
        EConfigure e;
        e.fun = std::bind(&DUDAQApp::configure, this, msg.param());
        DUFSM::sendEvent(e);
    }
    else if(cmd == "CONFONE") {
        duID = atoi((char*)msg.param());
    }
    else if(cmd == "STAR") {
        EStart e;
        e.fun = std::bind(&DUDAQApp::start, this);
        DUFSM::sendEvent(e);
    }
    else if(cmd == "DOTRIGGER") {
        // std::cout << "we will trigger now" << std::endl;
        m_dataManager->accept((char*)msg.param(), (size_t)msg.paramSize());
        // std::cout << "we will end trigger" << std::endl;
    }
    else if(cmd == "STOP") {
        EStop e;
        e.fun = std::bind(&DUDAQApp::stop, this);
        DUFSM::sendEvent(e);
    }
    else if(cmd == "TERM") {
        ETerminate e;
        e.fun = std::bind(&DUDAQApp::terminate, this);
        DUFSM::sendEvent(e);
    }

    int daqMode = m_server->m_duDAQMode;
    m_daqMode = daqMode;
    if(m_daqMode == 1) {
        m_frontend->setCallback([this](char *data, size_t sz)->void {
            this->m_dataManager->addRawEvent(data, sz); 
        });
    }
    if(m_daqMode == 2) {
        m_frontend->setCallback([this](char *data, size_t sz)->void {
            this->m_dataManager->addEvent(data, sz, m_daqMode);
        });
    }
    if(m_daqMode == 3) {
        m_frontend->setCallback([this](char *data, size_t sz)->void {
            this->m_dataManager->addEvent(data, sz, m_daqMode);
        });
    }
}

bool DUDAQApp::initialize() {
    m_frontend->initialize();
    return true;
}

bool DUDAQApp::configure(void *param) {
    m_frontend->configure(param);
    return true;
}

bool DUDAQApp::start() {
    m_frontend->start();
    return true;
}

bool DUDAQApp::stop() {
    m_frontend->stop();
    return true;
}

bool DUDAQApp::terminate() {
    m_frontend->terminate();
    return true;
}

bool DUDAQApp::toError() {
    return true;
}

