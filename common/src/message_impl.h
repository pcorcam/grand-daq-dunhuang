#pragma once 

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
    void *param();//read
    size_t paramSize();

    void setCmd(std::string cmd, void *param = nullptr, size_t paramSize = 0); //write to csdaq
    void copyFrom(char *data, size_t sz);
};

class AcceptMessage : public Message {
public:
    AcceptMessage(char *data, size_t sz, bool write = false);
    ~AcceptMessage();
    
    std::set<uint32_t> eventIDs(); //read

    void addEventID(uint32_t ID);
    void setEventIDs(std::set<uint32_t> &ids);
    void copyFrom(char *data, size_t sz);
};

class T2Message : public Message {

public:
    T2Message(char *data, size_t sz, bool write = true);
    ~T2Message();

    struct __attribute__((__packed__)) TQ {
        uint64_t time;
        uint32_t charge;
    };
    int m_szofT2 = 0;
    void addTQ(uint64_t t, uint32_t q);
    std::vector<TQ> TQData();
    void setTQData(char *eventData, size_t sz);
    void addTime(char *data, size_t sz);

private:

};

class T3Message : public Message //*************************
{
private:
    /* data */
public:
    T3Message(char *data, size_t sz, bool write = false);
    ~T3Message();
    
    // void selectT3Event();
};

class DAQEvent : public Message {
public:
    DAQEvent(char *data, size_t sz, bool write = false);
    void copyFrom(char *data, size_t sz);
    ~DAQEvent();
};

class RawEvent : public Message {
    public:
    RawEvent(char *data, size_t sz, bool write = false);
    void copyFrom(char *data, size_t sz);
    ~RawEvent();
};

}
