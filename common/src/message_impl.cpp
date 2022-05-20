#include "message_impl.h"

using namespace grand;

HandshakeMessage::HandshakeMessage(char *data, size_t sz, bool write) : Message(data, sz, write, MT_HSHAKE) {
} 

HandshakeMessage::~HandshakeMessage() {}

CommandMessage::CommandMessage(char *data, size_t sz, bool write) : Message(data, sz, write, MT_CMD) {
} 

CommandMessage::~CommandMessage() {}

std::string CommandMessage::cmd() {
    return data();
}

void CommandMessage::setCmd(std::string cmd)
{
    assert(size()+cmd.size()+1 <= m_size);
    memcpy(data(), cmd.c_str(), cmd.size());
    data()[cmd.size()] = '\0';
    setSizeByData(cmd.size()+1);
}

AcceptMessage::AcceptMessage(char *data, size_t sz, bool write) : Message(data, sz, write, MT_ACCEPT) {
}

AcceptMessage::~AcceptMessage() {}

std::set<uint32_t> AcceptMessage::eventIDs() {
    int numberofIDs = dataSize()/sizeof(uint32_t);
    std::set<uint32_t> eventIDs;
    for(int i=0; i<numberofIDs; i++) {
        eventIDs.insert(*(uint32_t*)(data()+4*i));
    }
    return std::move(eventIDs);
}

void AcceptMessage::addEventID(uint32_t id) {
    assert(size()+sizeof(uint32_t) <= m_size);
    char *ptr = data()+dataSize();
    *(uint32_t*)ptr = id;
    sizeInc(sizeof(uint32_t));
}

void AcceptMessage::setEventIDs(std::set<uint32_t> &ids) {
    assert(size() + sizeof(ids.size())*sizeof(uint32_t) <= m_size);
    int i=0;
    for(uint32_t id: ids) {
        ((uint32_t*)data())[i] = id;
    }
    setSizeByData(sizeof(ids.size())*sizeof(uint32_t));
}

T2Message::T2Message(char *data, size_t sz, bool write) : Message(data, sz, write, MT_T2) {
}

T2Message::~T2Message() {}

std::vector<T2Message::TQ> T2Message::TQData() {
    int num = dataSize()/sizeof(TQ);
    std::vector<T2Message::TQ> TQinfo;
    for(int i=0; i<num; i++) {
        TQ tq = *(TQ*)(data()+sizeof(TQ)*i);
        //std::cout << tq.time << std::endl;
        TQinfo.push_back(tq);
    }
    return TQinfo;
}

void T2Message::addTQ(uint64_t t, uint32_t q) {
    //std::cout << sizeof(TQ) << std::endl;
    assert(size()+12 <= m_size);
    TQ *tq = (TQ*)(data()+dataSize());
    tq->time = t;
    tq->charge = q;
    sizeInc(12);
}

void T2Message::setTQData(char *eventData, size_t sz) {
    assert("this function is not implemented");
}

DAQEvent::DAQEvent(char *data, size_t sz, bool write) :Message(data, sz, write, MT_DAQEVENT) {
}

DAQEvent::~DAQEvent() {}

void DAQEvent::copyFrom(char *data, size_t sz) {
    assert(size() + sz <= m_size);
    memcpy(this->data()+dataSize(), data, sz);
    sizeInc(sz);
}

