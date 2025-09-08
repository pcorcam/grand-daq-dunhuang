#pragma once

#include <iostream>
#include <functional>
#include <unistd.h>
#include <vector>
#include <string.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <list>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <zmq_server.h>
#include <du_sys_config.h>
#include <mutex>
#include <thread>
#include "data_format.h"
#include "message_impl.h"
#include "message_dispatcher.h"

#define timeCut 1000000000 // ns 
#define M_RINGBUFSZ 30000000 // // One comment, for the top limit of the FPGA is 1500Hz, the buffer sz should larger than 1500*8720=13M
#define M_BAKRINGBUFSZ 30000000
#define M_DUTIMESTAMPSZ 100000 // HERE! (M_RINGBUFSZ/szofEachEvent)*8 should less than M_DUTIMESTAMPSZ.
#define M_TIMEBUFFERSZ 100000
#define EACH_DATA_SZ 20
#define timeCount 1000000000 // ns
#define SZ_EVTBUFFER 1024000
#define M_EVENTBUF_SZ 20480
#define M_RAWEVENTBUF_SZ 20480

// MSG TYPE
#define TEN_SEC_DATA 32

// Position of data info
#define POS_TRIGGER_PATTERN 36

class XClock {
    public:

        inline static uint64_t nowNanoSeconds() {
            std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tp = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now());
            return std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
        }
};

namespace grand {

typedef std::function<void(char *data, size_t sz)> EventOutput;

class DataManager {
public:
    DataManager();
    ~DataManager();

    int get_ip_linux(int ipv4_6, std::list<std::string>& out_list_ip);
    int get_ipv4_linux(std::list<std::string>& out_list_ip4);

    void setEventOutput(EventOutput fun);
    void setT2EventOutput(EventOutput fun);
    void setRawEventOutput(EventOutput fun);

    /**
     * @brief called by readout thread
     *
     * @param data electronic event data
     */
    void addRawEvent(char *data, size_t sz);
    void addEvent(char *data, size_t sz, int daqMode);
    void lastAddEvent();
    void processCommand(char *data, size_t sz);
    /**
     * @brief called by backend command input thread
     *
     * @param accept message from cs-daq
     */
    void accept(char *data, size_t sz);
    
    void initialize();
    void start();
    void stop();
    void terminate();

private:
    
    EventOutput m_eventOutputFun; // Through setEventOutput function, m_eventOutputFun equals writefunc in m_server.
    EventOutput m_t2EventOutputFun; // Same as up.
    EventOutput m_rawEventOutputFun;

    ZMQServer *m_server;
    MessageDispatcher *m_msgDispatcher;
    
    struct t3Triggers{
        size_t sz;
        char buf[EACH_DATA_SZ];
    };

    // std::mutex m_mutex_IF;
    // std::mutex m_mutex_BA;
    std::mutex m_mutex;
    std::mutex mymutex;
    std::mutex m_mutex_data;
    
    char *m_eventBuffer; // stands for raw and t3event
    char *m_ringBuffer;
    char *m_event;
    char *m_rawEvent;
    char *m_duTimeStampSave;
    char *m_timeBuffer;
    char* m_tag;
    char* m_duNumbers;
    char *m_triggerTime;

    int m_rawDataSize = 0;
    int szofTimeBufferBak = 0;
    int m_count = 0;
    uint64_t m_time;
    
    int m_timeCount = 0;
    int szof_m_timeBuffer = 0;
    int szofRandomTrigger = 0;
    // size_t m_rawEventSz = 0;
    size_t m_eventBufferSize;
    size_t szof_m_duTimeStampSave = 0;
    
    size_t szofRingBuffer = 0;
    uint64_t m_timeStart = 0;
    int evtID1=0;
    int evtID2=0;
    int szT3Trigger = 0;
    int szSended = 0; 
    int szToClear = 0;
    int szSave = 0;

    uint64_t m_t0, m_t1, m_t2;
    size_t m_frequenceCountBefore;
    size_t m_frequenceCountAfter;

    char *m_bakRingBuffer;
    char *m_bakDuTimeStampSave;
    size_t szof_m_bakDuTimeStampSave = 0;
    size_t szofBakRingBuffer = 0;

    void acceptT3Trigger(char* data, size_t sz);
    void acceptRandomTrigger(char* data, size_t sz);
};

}

