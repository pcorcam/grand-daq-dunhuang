#pragma once

#include <client.h>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <assert.h>

namespace grand {

class ZMQClient: public Client {
public:
    ZMQClient();
    ~ZMQClient();

    void setup(int inputBufferSize, int zmqSndBufferSize);

    void write(std::string id, char* p, size_t sz);
    void initialize();
    void terminate();

protected:
    size_t read(char* p, size_t maxSz, std::string &ID);
    size_t readT2(char* p, size_t maxSz, std::string &ID);
private:
    int m_zmqSndBufferSize;

    std::mutex m_mutex;

    struct ClientItem {
        std::string ID;
        void *socket;
    };

    bool m_inited;
    void *m_context;
    void *m_poller;

    char m_daqMode[20] = {0};
    int m_csDAQMODE;

    std::map<std::string/*duID*/, ClientItem> m_clientItem;

};

}

