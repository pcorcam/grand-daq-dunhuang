#include <string.h>
#include <iostream>
#include <fstream>
#include <map>
#include <elec_config.h>

using namespace grand;

int main(){
    uint16_t sl[200000];

    std::string defaultConfig = ::getenv("GRAND_CSDAQ_CONFIG");
    std::string defaultConfigAddr = defaultConfig + "/DU-address-map.yaml";
    std::string defaultConfigData = defaultConfig + "/DU-readable-conf.yaml";

    ElecConfig *c = ElecConfig::instance();
    c->load(defaultConfigAddr, defaultConfigData);
    c->toShadowlist(&sl[0]);
    int j = 0;
    ofstream paras;
    paras.open("paras.txt");
    for(int i=0; i<505; i+=4){
	uint32_t reg_value;
	reg_value = sl[i] + (sl[i+1]<<8) + (sl[i+2]<<16) + (sl[i+3]<<24);
    	paras << dec << j << " "  <<  hex << i << " " << reg_value << endl;
       	j+=1;
    }
    paras.close();
    return 0;
}

