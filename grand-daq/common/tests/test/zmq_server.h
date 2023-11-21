#pragma once

#include <string>
#include "./server.h"

namespace grand {

class ZMQServer: public Server {
public:
    ZMQServer();
    ~ZMQServer();
    void setup(int inputBufferSize, std::string backendBindUrl, int maxClientAddressSize);
    
    size_t read(char* p, size_t maxSz);
    void write(char* p, size_t sz);
    void initialize();
    void terminate();

protected:

private:
    std::string m_backendBindUrl;
    int m_maxClientAddressSize;

    void *m_context;
    void *m_socket;
    char *m_address;
    void *m_poller;
    int m_addressSize;
};

}
