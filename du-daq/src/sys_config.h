#pragma once

#include <string>
#include <vector>

namespace grand {

class SysConfig {
public:
    static SysConfig *instance() {
        static SysConfig self;
        return &self;
    }

    uint32_t readoutBufferSize;
    uint32_t messageInputBufferSize;
    std::string backendBindUrl;
    uint32_t maxClientAddressSize;
private:

    SysConfig() {
        readoutBufferSize = 102400;
        messageInputBufferSize = 102400;
        backendBindUrl = "tcp://*:19555";
        maxClientAddressSize = 64;
    }
};

}
