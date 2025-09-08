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
#define DAQ_HEADER_SZ 12
#define T3TIMEBUF_EACHDU_MAP_SZ 50000
#define T3TIMEBUF_EACHDU_ALLSZ 50000
#define T3TIMEBUF_DOTIGGER_SZ 10000000

using namespace std;

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

    // Stop trigger processing and wait for completion
    void stop(bool init=true);

    // Check if trigger processing has stopped
    bool isStopped() const { return m_stopped.load(); }

    // Wait until all processing is complete
    void waitForCompletion();

    void processData(std::string du, char *data, size_t sz);
    
private:
    uint64_t t0, t1, t_start;
    int m_tmCount = 0;
    int m_bufferSize;
    int m_szt3TriggerBuf = 0;
    uint32_t m_tag = 0;
    char *m_t3TimeBuf;
    char *m_t3TriggerBuf;
    int m_daqMode;

    std::string m_defaultConfig;
    std::string m_defaultConfigSys;

    BufferPool *m_buffers;
    ThreadPool *m_threadPool;
    TriggerDone m_triggerDone;
    ZMQClient *m_client;

    // Distances between antennas
    std::unordered_map<uint16_t, std::unordered_map<uint16_t, double>> antenna_distance_map;

    // Multithreading
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_stopped{false};
    std::atomic<int> m_activeProcessors{0};

    struct evtInfo {
        size_t InfoSz;
        char buf[T3TIMEBUF_EACHDU_MAP_SZ];
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
        int m_t3TriggerType;
        bool m_dudaqFileInjection;
        string m_antennaDistancesFile;
    };
    T3paras paras;

    std::map<size_t, uint64_t> m_tmJudgement;
    std::map<size_t, std::map<std::size_t, evtInfo>> m_eventInfo;
    std::vector<std::pair<uint64_t, unsigned short>> du_events;
    std::map<size_t, size_t> m_trigerDU;
   
    void doTrigger(TriggerDone cb);
    void triggerAlgorithm(char *data, int sz, uint64_t tmID);
    void triggerAlgorithm2(bool process_young=false);
    void eachSecondCount(size_t duID, size_t sz);
    void doT3Trigger(std::string du, char* data, size_t sz);
    void doT3Trigger2(std::string du, char* data, size_t sz);
    void doLastT3Trigger();
    void randomTrigger(char* data, size_t sz);
    int genRand(int min, int max);
    // Read distances between antennas from a file
    int read_antenna_distances(string &filename);
    
    
};

}
