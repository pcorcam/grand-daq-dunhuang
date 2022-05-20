#include <cstdio>
#include <iostream>
#include <set>
#include <vector>
#include <string>
#include "message.h"
#include <cassert>

namespace grand {

class HandshakeMessage: public Message {
public:
    //void m_cmdAnalyze(char* data);
    HandshakeMessage(char *data, size_t sz, bool write = false); 
    ~HandshakeMessage();  
};

class CommandMessage: public Message {
public:
    //void m_cmdAnalyze(char* data);
    CommandMessage(char *data, size_t sz, bool write = false); 
    ~CommandMessage();  

    std::string cmd();//read
    void setCmd(std::string); //write to csdaq
};

class AcceptMessage : public Message {
public:
    AcceptMessage(char *data, size_t sz, bool write = false);
    ~AcceptMessage();
    
    std::set<uint32_t> eventIDs();//read

    void addEventID(uint32_t ID);
    void setEventIDs(std::set<uint32_t> &ids);
};

class T2Message : public Message {
public:
struct __attribute__((__packed__)) TQ {
    uint64_t time;
    uint32_t charge;
};

public:
    T2Message(char *data, size_t sz, bool write = false);
    ~T2Message();
    
    std::vector<TQ> TQData(); //read
    
    void addTQ(uint64_t t, uint32_t q);

    void setTQData(char *eventData, size_t sz);
    TQ *m_tqData;
};

class DAQEvent : public Message {
public:
    DAQEvent(char *data, size_t sz, bool write = false);
    void copyFrom(char *data, size_t sz);
    ~DAQEvent();
};

}
