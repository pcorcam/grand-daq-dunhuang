#pragma once

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <thread>
#include <cassert>
#include <functional>

namespace grand {

typedef std::function<void(char *, size_t)> ServerInputCallback;

class Server {
public:
    Server();
    virtual ~Server();

    void setCallback(ServerInputCallback cb);

    void inputThread();
    virtual void initialize();
    virtual void terminate();
    virtual void write(char* p, size_t sz) = 0;

protected:
    void setup(int inputBufferSize);

    virtual size_t read(char* p, size_t maxSz) = 0;
    bool m_stop;
    std::thread *m_thread;
   
private:
    int m_inputBufferSize;
    ServerInputCallback m_callback;
};

}
