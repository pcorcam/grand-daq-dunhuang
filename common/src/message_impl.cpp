/**************************/
// Creadted by duanbh,
// 20220423
/**************************/

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

void *CommandMessage::param() {
    std::string c = data();
    char *p = data()+c.size()+1;
    return (void*)p;
}

size_t CommandMessage::paramSize() {
    std::string c = data();
    return dataSize()-c.size()-1;
}

void CommandMessage::setCmd(std::string cmd, void *param, size_t paramSize)
{   
    // std::cout << "CommandMessage::setCmd" << std::endl;
    size_t cmdSize = cmd.size()+1;
    // printf("size(): %d, cmdSize: %d, paramSize: %d\n", size(), cmdSize, paramSize);
    // printf("size()+cmdSize+paramSize: %d, m_size: %d\n", size()+cmdSize+paramSize, m_size);
    assert(size()+cmdSize+paramSize <= m_size);
    
    memcpy(data(), cmd.c_str(), cmd.size());
    data()[cmd.size()] = '\0';

    if(param != nullptr) {
        memcpy(data()+cmdSize, param, paramSize);
    }

    // if(cmd=="DOTRIGGER") {
    //     char testtmstamp[20];
    //     int k = 0;
    //     for(int i=0; i<paramSize/20; i++){
    //         memset(testtmstamp, 0, 20);
    //         memcpy(testtmstamp, data()+cmdSize + k*20, 20);
    //         printf( "No.%d timestamp is %lld\n", k, atoll(testtmstamp));
    //         k++;
    //     }
    // }

    setSizeByData(cmdSize+paramSize);
    // // std::cout << "CommandMessage::setCmd" << std::endl;
}

void CommandMessage::copyFrom(char *data, size_t sz) {
    // // std::cout << "CommandMessage::copyFrom" << std::endl;
    assert(size() + sz <= m_size);
    memcpy(this->data()+dataSize(), data, sz);
    sizeInc(sz);
    // // std::cout << "CommandMessage::copyFrom end" << std::endl;
}

AcceptMessage::AcceptMessage(char *data, size_t sz, bool write) : Message(data, sz, write, MT_ACCEPT) {
}

AcceptMessage::~AcceptMessage() {}

std::set<uint32_t> AcceptMessage::eventIDs() {
    // std::cout << "AcceptMessage::eventIDs" << std::endl;
    int numberofIDs = dataSize()/sizeof(uint32_t);
    std::set<uint32_t> eventIDs;
    for(int i=0; i<numberofIDs; i++) {
        eventIDs.insert(*(uint32_t*)(data()+4*i));
    }
    return std::move(eventIDs);
    // std::cout << "AcceptMessage::eventIDs end" << std::endl;
}

void AcceptMessage::addEventID(uint32_t id) {
    // std::cout << "AcceptMessage::addEventID" << std::endl;
    assert(size()+sizeof(uint32_t) <= m_size);
    char *ptr = data()+dataSize();
    *(uint32_t*)ptr = id;
    sizeInc(sizeof(uint32_t)); // Expand header size.
    // std::cout << "AcceptMessage::addEventID end" << std::endl;
}

void AcceptMessage::setEventIDs(std::set<uint32_t> &ids) {
    // std::cout << "AcceptMessage::setEventIDs" << std::endl;
    assert(size() + sizeof(ids.size())*sizeof(uint32_t) <= m_size);
    int i=0;
    for(uint32_t id: ids) {
        ((uint32_t*)data())[i] = id;
    }
    setSizeByData(sizeof(ids.size())*sizeof(uint32_t));
    // std::cout << "AcceptMessage::setEventIDs end" << std::endl;
}

void AcceptMessage::copyFrom(char *data, size_t sz) {
    // std::cout << "AcceptMessage::copyFrom" << std::endl;
    assert(size() + sz <= m_size);
    memcpy(this->data()+dataSize(), data, sz);
    sizeInc(sz);
    // std::cout << "AcceptMessage::copyFrom end" << std::endl;
}

T2Message::T2Message(char *data, size_t sz, bool write) : Message(data, sz, write, MT_T2) {
}

T2Message::~T2Message() {}

std::vector<T2Message::TQ> T2Message::TQData() {
    int num = dataSize()/sizeof(TQ);
    std::vector<T2Message::TQ> TQinfo;
    for(int i=0; i<num; i++) {
        TQ tq = *(TQ*)(data()+sizeof(TQ)*i);
        //// std::cout << tq.time << std::endl;
        TQinfo.push_back(tq);
    }
    return TQinfo;
}

void T2Message::addTQ(uint64_t t, uint32_t q) {
    assert(size()+12 <= m_size);
    TQ *tq = (TQ*)(data()+dataSize());
    tq->time = t;
    tq->charge = q;
    sizeInc(12);
}

void T2Message::setTQData(char *eventData, size_t sz) {
    assert("this function is not implemented");
}

void T2Message::addTime(char *data, size_t sz) {
    assert(size() + sz <= m_size);
    memcpy(this->data()+dataSize(), data, sz);
    sizeInc(sz);
}

DAQEvent::DAQEvent(char *data, size_t sz, bool write) :Message(data, sz, write, MT_DAQEVENT) {
}

DAQEvent::~DAQEvent() {}

void DAQEvent::copyFrom(char *data, size_t sz) {
    // std::cout << "DAQEvent::copyFrom"<< std::endl;
    assert(size() + sz <= m_size);
    memcpy(this->data()+dataSize(), data, sz);
    sizeInc(sz);
    // std::cout << "DAQEvent::copyFrom end;"<< std::endl;
}

RawEvent::RawEvent(char *data, size_t sz, bool write) : Message(data, sz, write, MT_RAWEVENT) {
}

RawEvent::~RawEvent() {}

void RawEvent::copyFrom(char *data, size_t sz) {
    // // std::cout << "RawEvent::copyFrom"<< std::endl;
    assert(size() + sz <= m_size);
    memcpy(this->data()+dataSize(), data, sz);
    sizeInc(sz);
    // // std::cout << "RawEvent::copyFrom end"<< std::endl;
}

