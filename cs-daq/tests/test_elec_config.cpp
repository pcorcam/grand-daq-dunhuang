#include <string.h>
#include <iostream>
#include <fstream>
#include <map>
#include <elec_config.h>

using namespace grand;

#define SHADOW_SIZE 2000

int main(){
    uint16_t sl[SHADOW_SIZE];

    std::string defaultConfig = ::getenv("GRAND_DAQ_CONFIG");
    std::string defaultConfigAddr = defaultConfig + "/DU-address-map.yaml";
    std::string defaultConfigData = defaultConfig + "/DU-readable-conf.yaml";

    ElecConfig *c = ElecConfig::instance();
    c->load(defaultConfigAddr, defaultConfigData);
    c->toShadowlist(&sl[0]);
    int j = 0;
    ofstream params;
    params.open("/tmp/shadow_list.dat", std::ofstream::binary);
    params.write((char*)sl, SHADOW_SIZE);
    
    ofstream paras;
    paras.open("paras.txt");
    for(int i=0; i<505; i+=4){
	uint32_t reg_value;
	reg_value = sl[i] + (sl[i+1]<<8) + (sl[i+2]<<16) + (sl[i+3]<<24);
    	paras << dec << j << " "  <<  hex << i << " " << reg_value << endl;
       	j+=1;
    }

    paras.close();
    params.close();
    return 0;
}

