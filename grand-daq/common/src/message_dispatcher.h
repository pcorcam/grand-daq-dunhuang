#pragma once 

#include <message.h>
#include <string>
#include <map>
#include <types.h>

namespace grand {

class MessageDispatcher {
public:
    MessageDispatcher();

    void dispatch(std::string duID, char* data, size_t sz);
    void dispatch(char* data, size_t sz);

    void addProcessor(MessageType type, MessageProcessorA mp);
    void addProcessor(MessageType type, MessageProcessorB mp);

private:
    std::map<MessageType, MessageProcessorA> m_processorsA;
    std::map<MessageType, MessageProcessorB> m_processorsB;
};

}
