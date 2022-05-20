#include <message_dispatcher.h>

using namespace grand;

MessageDispatcher::MessageDispatcher() {
}

void MessageDispatcher::operator()(std::string duID, char* data, size_t sz) {
    Message msg(data, sz);
    MessageType type = msg.type();
    if(m_processors.count(type)) {
        m_processors[type](duID, data, sz);
    }
}

void MessageDispatcher::addProcessor(MessageType type, MessageProcessor mp) {
    m_processors[type] = mp;
}

