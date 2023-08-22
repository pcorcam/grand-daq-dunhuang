#include <string.h>
#include <iostream>
#include <fstream>
#include <map>
#include <cs_sys_config.h>
#include <cassert>

using namespace grand;

void dumpDU(std::vector<DUConfig> &cfgs) 
{
    for(auto du: cfgs) {
        std::cout << "---" << std::endl;
        std::cout << du.ID << std::endl;
        std::cout << du.type << std::endl;
        std::cout << du.ip << std::endl;
        std::cout << du.port << std::endl;
    }
}

void dumpGlobal(CSSysConfig *c) {
    std::cout << "global zmqSndBufferSize = " << c->appConfig().zmqSndBufferSize << std::endl;
}

int main() {
    std::string defaultConfig = ::getenv("GRAND_DAQ_CONFIG");
    std::string file = defaultConfig + "/sysconfig.yaml";

    CSSysConfig *c = CSSysConfig::instance();
    c->load(file);
    assert(c->duConfigs().size() > 0);
    assert(c->appConfig().zmqSndBufferSize > 0 && c->appConfig().zmqSndBufferSize < 4096);

    dumpGlobal(c);
    dumpDU(c->duConfigs());
    return 0;
}

