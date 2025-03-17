#pragma once

#include <string.h>
#include <functional>
#include <iostream>
#include "yaml-cpp/yaml.h"
#include "yaml-cpp/node/parse.h"
#include <fstream>
#include <map>
#include <assert.h>

 // #define Reg_End 0x1FC
#define Reg_End 0x2FC

using namespace std; 

namespace grand {

class ElecConfigAddress
{
public:
     typedef struct{
        uint32_t baseAddr ;
        uint32_t startBit ;
        uint32_t nBits ; 
    }addr_t;

    addr_t global(string group, string name);
    addr_t channel(string channelName, string name);
};

class ElecConfig
{
public:
    static ElecConfig* instance();
    void load(string addressFile, std::string dataFile);
    size_t toShadowlist(uint8_t *sl);
    size_t toShadowlist(uint8_t *sl, std::string DUid);

private:
    map<string, std::function<double(double)>> m_transformFunction;
    map<string, std::function<void(double*, size_t, uint16_t*, size_t&)>> m_transformFunctionArray;
    map<string, std::function<uint32_t(uint32_t)>> m_transformFunction_2;
    YAML::Node m_config;
    YAML::Node n_config;
    ElecConfigAddress m_configAddress;
    
    bool COMMON, SPECIAL;

    ElecConfig();
    std::function<uint32_t(uint32_t)> transformFunction(string first, string second, string third);
    std::function<double(double)> transformFunction2(string first, string second, string third);
    std::function<void(double*, size_t, uint16_t*, size_t&)> transformFunctionArray(string first, string second, string third);
    std::function<uint32_t(uint32_t)> transformFunction_2(string first);
    void setBit(uint8_t *sl, uint32_t baseAddr, uint32_t startBit, uint32_t value);
    void setBits(uint8_t *sl, uint32_t baseAddr, uint32_t startBit, uint32_t nBits, uint32_t value);
    void setBits(uint8_t *sl, uint32_t baseAddr, uint32_t startBit, uint32_t nBits, uint16_t *value);
    void setBits(uint8_t *sl, uint32_t baseAddr, uint32_t startBit, uint32_t nBits, double *value);
    int float2fixed(float x, int len_int, int len_frac);
    uint32_t fundefault(uint32_t value);
    void fundefaultArray(double *value, size_t sz, uint16_t*, size_t&);
    uint32_t funInternalTriggerRate(uint32_t value);
    uint32_t funTriggerOverlap(uint32_t value);
    uint32_t funTriggerBlock(uint32_t value);
    uint32_t funBattery(uint32_t value);
    uint32_t funInput_Off(uint32_t value);
    uint32_t funInput_ADC(uint32_t value);
    uint32_t funPreorPostTri(uint32_t value);
    uint32_t funQuiettime(uint32_t value);
    uint32_t funtimeAfter(uint32_t value);
    uint32_t funMaxTime(uint32_t value);
    double funAdditionaGain(double value);
    void funIIR(double *value, size_t sz, uint16_t* values, size_t& length);
};

}
