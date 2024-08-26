#include <cs_sys_config.h>
#include <vector>
#include <iostream>

using namespace std;
using std::cout;
using std::endl;

using namespace grand;

#define SYS_CONFIG_LOAD_GLOBAL(NAME, TYPE) m_appConfig.NAME = config["global"][#NAME].as<TYPE>()

CSSysConfig *CSSysConfig::instance() {
    static CSSysConfig self;
    // printf("trigger DU number is %d\n", self.appConfig().t3TriggerDuNumber);
    return &self;
}

CSSysConfig::CSSysConfig() {
}

void CSSysConfig::load(std::string file) {
    YAML::Node config = YAML::LoadFile(file);
    SYS_CONFIG_LOAD_GLOBAL(networkInputBufferSize, uint32_t);
    SYS_CONFIG_LOAD_GLOBAL(zmqSndBufferSize, uint32_t);
    SYS_CONFIG_LOAD_GLOBAL(eventBufferPageSize, uint32_t);
    SYS_CONFIG_LOAD_GLOBAL(eventBufferNumberOfPages, uint32_t);
    SYS_CONFIG_LOAD_GLOBAL(eventBufferPageSize, uint32_t);
    SYS_CONFIG_LOAD_GLOBAL(eventBufferNumberOfPages, uint32_t);
    SYS_CONFIG_LOAD_GLOBAL(t3TriggerTimeCut, uint32_t);
    SYS_CONFIG_LOAD_GLOBAL(t3TriggerTimeOut, uint32_t);
    SYS_CONFIG_LOAD_GLOBAL(t3TriggerTimeWindow, uint32_t);
    // SYS_CONFIG_LOAD_GLOBAL(t3TriggerThreshold, uint32_t);
    SYS_CONFIG_LOAD_GLOBAL(t3TriggerDuNumber, uint32_t);
    SYS_CONFIG_LOAD_GLOBAL(l1FileName, string);
    SYS_CONFIG_LOAD_GLOBAL(l3FileName, string);
    SYS_CONFIG_LOAD_GLOBAL(daqMode, uint32_t);
    SYS_CONFIG_LOAD_GLOBAL(eventNumberSaved, uint32_t);
    
    // m_appConfig.zmqSndBufferSize = config["global"]["zmqSndBufferSize"].as<uint32_t>();
    // m_appConfig.eventBufferPageSize = config["global"]["eventBufferPageSize"].as<uint32_t>();
    // m_appConfig.eventBufferNumberOfPages = config["global"]["eventBufferNumberOfPages"].as<uint32_t>();
    // m_appConfig.eventBufferPageSize = config["global"]["eventBufferPageSize"].as<uint32_t>();
    // m_appConfig.eventBufferNumberOfPages = config["global"]["eventBufferNumberOfPages"].as<uint32_t>();
    for(auto du: config["dataUnits"]) {
        DUConfig duCfg;
        duCfg.type = du["type"].as<std::string>();
        duCfg.ID = du["ID"].as<std::string>();
        duCfg.ip = du["ip"].as<std::string>();
        duCfg.port = du["port"].as<uint32_t>();
        m_duConfigs.push_back(std::move(duCfg));
    }
}
