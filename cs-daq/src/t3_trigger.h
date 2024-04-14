#pragma once

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <utils.h>
#include <zmq_client.h>
#include <yaml-cpp/yaml.h>
#include "yaml-cpp/node/parse.h"
#include "map"

#define EACH_DATA_SZ 20

class XXClock {
    public:
        inline static uint64_t nowNanoSeconds() {
            std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tp = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now());
            return std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
        }
};

namespace grand {

typedef std::function<void ()> TriggerDone;

class T3Trigger {
public:
    T3Trigger(TriggerDone triggerDoneCallback = nullptr, ZMQClient *client = nullptr);
    ~T3Trigger();

    void stop(); // trigger within the last second.
    void processData(std::string du, char *data, size_t sz);
    
private:
    uint64_t *m_t3Container;
    uint64_t t0, t1, t_start;
    int m_tmCount = 0;
    int m_bufferSize;
    int m_szt3TriggerBuf = 0;
    uint32_t m_tag = 0;
    char *m_t3TimeBuf;
    char *m_t3TriggerBuf;
    char *m_t3TimeContainer;
    int m_daqMode;

    std::string m_defaultConfig;
    std::string m_defaultConfigSys;

    BufferPool *m_buffers;
    ThreadPool *m_threadPool;
    TriggerDone m_triggerDone;
    ZMQClient *m_client;

    struct evtInfo {
        size_t InfoSz;
        char buf[100000];
    };

    struct t3TmDuid {
        size_t sz;
        char* buf;
    };

    struct  T3paras {
        int m_timeCut;
        int m_timeOut;
        int m_timeWindow;
        int m_triggerThreshold;
    };
    T3paras paras;

    std::map<size_t, uint64_t> m_tmJudgement;
    std::map<size_t, std::map<std::size_t, evtInfo>> m_eventInfo;
    std::map<size_t, size_t> m_trigerDU;
   
    void doTrigger(TriggerDone cb);
    void triggerAlgorithm(char *data, int sz, uint64_t tmID);
    void eachSecondCount(size_t duID, size_t sz);
    void doT3Trigger(std::string du, char* data, size_t sz);
    void randomTrigger(char* data, size_t sz);
    int genRand(int min, int max);
    void doLastT3Trigger();
    
};

}
