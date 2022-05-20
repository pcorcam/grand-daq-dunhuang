#include <elec_config.h>
#include <string>
#include <fstream>
#include <math.h>
#include <vector>
#include <algorithm>
#include <functional>
#include <malloc.h>

using namespace std;
using namespace grand;

ElecConfig* ElecConfig::instance() 
{
    static ElecConfig self;
    return &self;
}

ElecConfig::ElecConfig() {
}

ElecConfigAddress::addr_t ElecConfigAddress::global(string group, string name)  //define global
{
    return ElecConfigAddress::addr_t();
}

ElecConfigAddress::addr_t ElecConfigAddress::channel(string channelName, string name)  //define channel
{
    return ElecConfigAddress::addr_t();
}

uint32_t ElecConfig::fundefault(uint32_t value){return value;}
void ElecConfig::fundefaultArray(double* value, size_t size, uint16_t*, size_t&) { 
    std::cout << 0 << std::endl;
}

uint32_t ElecConfig::funTest_Pulse(uint32_t value)
{
    if(value == 0){
        return value+=15;
    }

    if(value != 0){
        uint32_t value1 = value<<8;
        return value1+=((1<<7)+15);
    }
    return 0;
}

uint32_t ElecConfig::funTriggerOverlap(uint32_t value) {return value/2;}
uint32_t ElecConfig::funInput_Off(uint32_t value) {return value*0;}
uint32_t ElecConfig::funInput_ADC(uint32_t value) {return value-1;}
uint32_t ElecConfig::funPreorPostTri(uint32_t value){return value/2;}
uint32_t ElecConfig::funQuiettime(uint32_t value){return value/4;}
uint32_t ElecConfig::funtimeAfter(uint32_t value){return value/16;}
uint32_t ElecConfig::funMaxTime(uint32_t value){return value/4;}

int ElecConfig::float2fixed(float x, int len_int, int len_frac){
    int f;
    if (len_int + len_frac > 32) {
        printf("ERROR: too many bits!  Must fit in 32b int!\n");
        return 0;
    }
    // Round-to-nearest
    f = (int)(x*(1 << len_frac) + (x < 0 ? -1.0 : 1.0)*0.5);
    f &= 0xffffffff >> (32-len_int-len_frac);

    return f;
}

void ElecConfig::funIIR(double *value, size_t sz, uint16_t* values, size_t& length)
{
    int a[10],b[10];
    double a_dbl[10], b_dbl[10];
    int SAMP_FREQ = 500;
    double nu_s;
    int W_INT = 22;
    int W_FRAC = 10;
    double freq, width;
    freq = value[0];
    width = value[1];
    nu_s = freq/SAMP_FREQ;
    a_dbl[1] = 2 * width * width * width * width * cos(4*2*3.1416*nu_s);
    a_dbl[2] = -width*width*width*width*width*width*width*width;
    b_dbl[0] = 1;
    b_dbl[1] = -2 * cos(2*3.1416*nu_s);
    b_dbl[2] = 1;
    b_dbl[3] = 2 * width * cos(2*3.1416*nu_s);
    b_dbl[4] = width*width;
    b_dbl[5] = 2 * width * width * cos(2*2*3.1416*nu_s);
    b_dbl[6] = width*width*width*width;

    a[1] = float2fixed(a_dbl[1], W_INT, W_FRAC);
    a[2] = float2fixed(a_dbl[2], W_INT, W_FRAC);

    for (int i = 0; i < 7; i++)
        b[i] = float2fixed(b_dbl[i], W_INT, W_FRAC);

    int m = 1;
    int n = 1;
    int j = 0;
    for(int i=0; i<8; i++){
        if(i < 2){
            values[j] = a[m]&0xffff;
            m++;
        }
        else{
            values[j] = b[n]&0xffff;
            n++;
        }
        j++;
    }
}

uint32_t ElecConfig::fun_2default(uint32_t value){return value;}

void ElecConfig::load(std::string addressFile, std::string dataFile)
{
    m_config = YAML::LoadFile(addressFile);
    n_config = YAML::LoadFile(dataFile);

    m_transformFunction["default"] = std::bind(&ElecConfig::fundefault, this, std::placeholders::_1);
    m_transformFunction["Global|Test Pulse|0-255:0=0Hz,1=1MHz,255=124Hz"] = std::bind(&ElecConfig::funTest_Pulse, this, std::placeholders::_1);
    m_transformFunction["Global|TriggerOverlap|Time(ns)"] = std::bind(&ElecConfig::funTriggerOverlap, this, std::placeholders::_1);
    m_transformFunction["Input|Ch1|Off"] = std::bind(&ElecConfig::funInput_Off, this, std::placeholders::_1);
    m_transformFunction["Input|Ch1|ADC"] = std::bind(&ElecConfig::funInput_ADC, this, std::placeholders::_1);
    m_transformFunction["Input|Ch2|Off"] = std::bind(&ElecConfig::funInput_Off, this, std::placeholders::_1);
    m_transformFunction["Input|Ch2|ADC"] = std::bind(&ElecConfig::funInput_ADC, this, std::placeholders::_1);
    m_transformFunction["Input|Ch3|Off"] = std::bind(&ElecConfig::funInput_Off, this, std::placeholders::_1);
    m_transformFunction["Input|Ch3|ADC"] = std::bind(&ElecConfig::funInput_ADC, this, std::placeholders::_1);
    m_transformFunction["Input|Ch4|Off"] = std::bind(&ElecConfig::funInput_Off, this, std::placeholders::_1);
    m_transformFunction["Input|Ch4|ADC"] = std::bind(&ElecConfig::funInput_ADC, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 1|Pre Trigger"] = std::bind(&ElecConfig::funPreorPostTri, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 1|Post Trigger"] = std::bind(&ElecConfig::funPreorPostTri, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 1|Quiet Time before Sig treshold(ns)"] = std::bind(&ElecConfig::funQuiettime, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 1|Time after Sig threshold(ns)"] = std::bind(&ElecConfig::funtimeAfter, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 1|Max Time between threshold crossings(ns)"] = std::bind(&ElecConfig::funMaxTime, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 2|Pre Trigger"] = std::bind(&ElecConfig::funPreorPostTri, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 2|Post Trigger"] = std::bind(&ElecConfig::funPreorPostTri, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 2|Quiet Time before Sig treshold(ns)"] = std::bind(&ElecConfig::funQuiettime, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 2|Time after Sig threshold(ns)"] = std::bind(&ElecConfig::funtimeAfter, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 2|Max Time between threshold crossings(ns)"] = std::bind(&ElecConfig::funMaxTime, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 3|Pre Trigger"] = std::bind(&ElecConfig::funPreorPostTri, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 3|Post Trigger"] = std::bind(&ElecConfig::funPreorPostTri, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 3|Quiet Time before Sig treshold(ns)"] = std::bind(&ElecConfig::funQuiettime, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 3|Time after Sig threshold(ns)"] = std::bind(&ElecConfig::funtimeAfter, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 3|Max Time between threshold crossings(ns)"] = std::bind(&ElecConfig::funMaxTime, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 4|Pre Trigger"] = std::bind(&ElecConfig::funPreorPostTri, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 4|Post Trigger"] = std::bind(&ElecConfig::funPreorPostTri, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 4|Quiet Time before Sig treshold(ns)"] = std::bind(&ElecConfig::funQuiettime, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 4|Time after Sig threshold(ns)"] = std::bind(&ElecConfig::funtimeAfter, this, std::placeholders::_1);
    m_transformFunction["Channels|Channel 4|Max Time between threshold crossings(ns)"] = std::bind(&ElecConfig::funMaxTime, this, std::placeholders::_1);

    m_transformFunctionArray["default"] = std::bind(&ElecConfig::fundefaultArray, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 1|Filter1"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 1|Filter2"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 1|Filter3"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 1|Filter4"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 2|Filter1"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 2|Filter2"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 2|Filter3"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 2|Filter4"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 3|Filter1"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 3|Filter2"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 3|Filter3"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 3|Filter4"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 4|Filter1"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 4|Filter2"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 4|Filter3"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_transformFunctionArray["Channels|Channel 4|Filter4"] = std::bind(&ElecConfig::funIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

    m_transformFunction_2["Station Rate(Simulation)"] = std::bind(&ElecConfig::fun_2default, this, std::placeholders::_1);
}

void ElecConfig::setBit(uint16_t *sl, uint32_t baseAddr, uint32_t startBit, uint32_t value)
{
    if(value == 0) {
        sl[baseAddr] &= (~(0x01<<startBit));
    }
    else {
        sl[baseAddr] |= (0x01<<startBit);
    }
}

void ElecConfig::setBits(uint16_t *sl, uint32_t baseAddr, uint32_t startBit, uint32_t nBits, uint32_t value)
{
    uint32_t value1= value;
    for(int i=0; i<nBits; i++) {
        int iByte = (i+startBit)/8;
        int iBit = (i+startBit)%8;
        int newBaseAddr = baseAddr + iByte;
        int newValue = (value1>>i)&0x01;
        setBit(sl, newBaseAddr, iBit, newValue);
    }
}

void ElecConfig::setBits(uint16_t *sl, uint32_t baseAddr, uint32_t startBit, uint32_t nBits, uint16_t *value)
{
    int nloop = nBits/16;
    uint32_t value1;
    for(int i=0; i<nloop; i++){
        value1 = value[i];
        startBit = i*16;
        setBits(sl, baseAddr, startBit, 16, value1);
    }
}

std::function<uint32_t(uint32_t)> ElecConfig::transformFunction(string first, string second, string third)
{
    string key = first + "|" + second + "|" + third;
    if(m_transformFunction.count(key) > 0) {  //to pick same function from ElecConfig::ElecConfigpath(string configPath)
        return m_transformFunction[first + "|" + second + "|" + third];
    }
    else {
        return m_transformFunction["default"];
    }
};

std::function<void(double*, size_t, uint16_t*, size_t&)> ElecConfig::transformFunctionArray(string first, string second, string third)
{
    string key = first + "|" + second + "|" + third;
    if(m_transformFunctionArray.count(key) > 0) {  //to pick same function from ElecConfig::ElecConfigpath(string configPath)
        return m_transformFunctionArray[first + "|" + second + "|" + third];
    }
    else {
        return m_transformFunctionArray["default"];
    }
};

std::function<uint32_t(uint32_t)> ElecConfig::transformFunction_2(string first)
{
    string key = first;
    if(m_transformFunction_2.count(key) > 0) {  //to pick same function from ElecConfig::ElecConfigpath(string configPath)
        return m_transformFunction_2[first];
    }
    else {
        return m_transformFunction_2["default"];
    }
};

void ElecConfig::toShadowlist(uint16_t *sl)
{   
    ElecConfigAddress::addr_t addr;
    int SHADOW_SIZE = 8;
    memset(sl, 0, SHADOW_SIZE);
    YAML::Node::const_iterator node1;
    YAML::Node::const_iterator reg_info;
    for(node1 = n_config.begin(); node1 != n_config.end(); node1++){
        string group = node1->first.as<string>();
        if(n_config[group].Type() == YAML::NodeType::Scalar){
            uint32_t value = n_config[group].as<int>();
            uint32_t value1 = transformFunction_2(group)(value);
            addr.baseAddr = m_config[group]["baseAddr"].as<int>();
            addr.startBit = m_config[group]["startBit"].as<int>();
            addr.nBits = m_config[group]["nBits"].as<int>();
            setBits(sl, addr.baseAddr, addr.startBit, addr.nBits, value1);
        }
        YAML::Node::const_iterator node2;
        for(node2 = n_config[group].begin(); node2 != n_config[group].end(); node2++){
            string name = node2->first.as<string>();
            YAML::Node::const_iterator node3;
            for(node3 = n_config[group][name].begin(); node3 != n_config[group][name].end(); node3++){
                string name2 = node3->first.as<string>();
                addr.baseAddr = m_config[group][name][name2]["baseAddr"].as<int>();
                addr.startBit = m_config[group][name][name2]["startBit"].as<int>();
                addr.nBits = m_config[group][name][name2]["nBits"].as<int>();
                if(n_config[group][name][name2].Type() == YAML::NodeType::Sequence) {
                    YAML::Node::const_iterator node_array;
                    int i=0;
                    int seqSize = n_config[group][name][name2].size();
                    double array[seqSize];
                    for(node_array = n_config[group][name][name2].begin();node_array != n_config[group][name][name2].end(); node_array++){
                        array[i] = n_config[group][name][name2][i].as<double>();
                        i++;
                    }
                    int size = i+1;
                    uint16_t filter[8];
                    size_t szRet = size;
                    transformFunctionArray(group, name, name2)( array, size, filter, szRet);
                    setBits(sl, addr.baseAddr, addr.startBit, addr.nBits, filter);
                    continue;
                }
                uint32_t value = node3->second.as<int>();
                uint32_t value1 = transformFunction(group, name, name2)(value); //just modify the value
                setBits(sl, addr.baseAddr, addr.startBit, addr.nBits, value1);
            }
        }
    }
}


