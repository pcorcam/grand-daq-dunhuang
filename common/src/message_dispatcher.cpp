#include <message_dispatcher.h>
#include <utils.h>

using namespace grand;

MessageDispatcher::MessageDispatcher() {
}

void MessageDispatcher::dispatch(std::string duID, char* data, size_t sz) {
    Message msg(data, sz);
    MessageType type = msg.type();
    CLOG(DEBUG, "network") << "dispatch type = " << type << ", sz = " << sz;
    if(m_processorsA.count(type)) {
        m_processorsA[type](duID, data, sz);
    }
}

void MessageDispatcher::dispatch(char* data, size_t sz) {
    Message msg(data, sz);
    MessageType type = msg.type();
    CLOG(DEBUG, "network") << "dispatch type = " << type << ", sz = " << sz;
    if(m_processorsB.count(type)) {
        m_processorsB[type](data, sz);
    }
}

void MessageDispatcher::addProcessor(MessageType type, MessageProcessorA mp) {
    m_processorsA[type] = mp;
}

void MessageDispatcher::addProcessor(MessageType type, MessageProcessorB mp) {
    m_processorsB[type] = mp;
}

