#pragma once 

#include <message.h>
#include <string>
#include <map>
#include <types.h>

namespace grand {

class MessageDispatcher {
public:
    MessageDispatcher();

    void operator()(std::string duID, char* data, size_t sz);

    void addProcessor(MessageType type, MessageProcessor mp);

private:
    std::map<MessageType, MessageProcessor> m_processors;
};

}
