#include <message_dispatcher.h>
#include <utils.h>

using namespace grand;

MessageDispatcher::MessageDispatcher() {
}

void MessageDispatcher::dispatch(std::string duID, char* data, size_t sz) {
    // std::cout << "MessageDispatcher::dispatch(char* data, char* data, size_t sz)" << std::endl;
    Message msg(data, sz);
    MessageType type = msg.type();
    CLOG(DEBUG, "network") << "dispatch type = " << type << ", sz = " << sz;
    if(m_processorsA.count(type)) {
        m_processorsA[type](duID, data, sz);
        
    }
    // std::cout << "MessageDispatcher::dispatch(char* data, char* data, size_t sz)" << std::endl;
}

void MessageDispatcher::dispatch(char* data, size_t sz) {
    // std::cout << "MessageDispatcher::dispatch(char* data, size_t sz)" << std::endl;
    Message msg(data, sz);
    MessageType type = msg.type();
    CLOG(DEBUG, "network") << "dispatch type = " << type << ", sz = " << sz;
    if(m_processorsB.count(type)) {
        m_processorsB[type](data, sz);
    }
    // std::cout << "MessageDispatcher::dispatch(char* data, size_t sz) end" << std::endl;
}

void MessageDispatcher::addProcessor(MessageType type, MessageProcessorA mp) {
    m_processorsA[type] = mp;
}

void MessageDispatcher::addProcessor(MessageType type, MessageProcessorB mp) {
    m_processorsB[type] = mp;
}

