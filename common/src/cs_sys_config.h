#pragma once

#include <string>
#include <yaml-cpp/yaml.h>
#include "yaml-cpp/node/parse.h"
#include <vector>

namespace grand {

struct DUConfig {
    std::string ID;
    std::string type;
    std::string ip;
    uint32_t port;
};

struct APPConfig {
    int networkInputBufferSize;
    int zmqSndBufferSize;
    int t2BufferPageSize;
    int t2BufferNumberOfPages;
    int eventBufferPageSize;
    int eventBufferNumberOfPages;
    int t3TriggerTimeCut;
    int t3TriggerTimeOut;
    int t3TriggerTimeWindow;
    // int t3TriggerThreshold;
    int t3TriggerDuNumber;
    int daqMode;
    int eventNumberSaved;
};

class CSSysConfig {
public:
    static CSSysConfig *instance();
    void load(std::string file);

    std::vector<DUConfig> &duConfigs() {
        return m_duConfigs;
    }

    APPConfig &appConfig() {
        return m_appConfig;
    }

private:
    std::vector<DUConfig> m_duConfigs;
    APPConfig m_appConfig;
    int m_zmqSndBufferSize;
    CSSysConfig();
};

}
