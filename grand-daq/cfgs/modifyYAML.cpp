#include <iostream>
#include "yaml-cpp/yaml.h"
#include <string>
#include <fstream>

using std::ofstream;

int main(int argc, char** argv) {
    //< 改
    YAML::Node config = YAML::LoadFile("./DU-readable-conf.yaml");
    ofstream fout("./DU-readable-conf.yaml"); //< C++ stream 读取文件

    config["CommonConfig"]["Channels"]["Channel 1"]["Additional Gain[-14,23.5](dB)"] = atoi(argv[1]);   //< 修改格式 1
    config["CommonConfig"]["Channels"]["Channel 2"]["Additional Gain[-14,23.5](dB)"] = atoi(argv[1]);
    config["CommonConfig"]["Channels"]["Channel 3"]["Additional Gain[-14,23.5](dB)"] = atoi(argv[1]);
    config["CommonConfig"]["Channels"]["Channel 4"]["Additional Gain[-14,23.5](dB)"] = atoi(argv[1]);
    // config["name"] = "hening"; //< 修改格式2

    fout << config;

    fout.close();

    return 0;
}
