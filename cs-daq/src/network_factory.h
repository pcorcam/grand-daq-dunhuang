#pragma once 

#include <zmq_network.h>

namespace grand {

class NetworkFactory {
public:
    static INetwork *get() {
        static ZMQNetwork self;
        return &self;
    }
};

}
