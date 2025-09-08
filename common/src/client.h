#pragma once 

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <thread>
#include <cassert>
#include <vector>
#include <list>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

#define MAX_INPUT_DATA_SIZE (1024*1024)

namespace grand {

typedef std::function<void(std::string id, char* data, size_t sz)> ClientInputCallback;

class Client {
public:
    Client();
    virtual ~Client();

    void addClient(std::string id ,std::string ip, uint32_t port);
    void addCallback(ClientInputCallback cb); // ******


    virtual void initialize();
    virtual void terminate();

    void writeOne(std::string duID, char *p, size_t sz);
    void writeAll(char *p, size_t sz);
    void waitForWriteAllCompletion();

protected:
    void setup(int inputBufferSize);

    struct ClientInfo {
        std::string ID;
        std::string ip;
        uint32_t port;
    };

    std::vector<ClientInfo> m_clientInfo;
    
    virtual void write(std::string duID, char* p, size_t sz) = 0;
    virtual size_t read(char* p, size_t maxSz, std::string &duID) = 0;
    void inputThread();

private:
    int m_inputBufferSize;
    int m_t2inputBufferSize;
    std::thread *m_thread;
    bool m_stop;
    char *m_buffer;

    std::list<ClientInputCallback> m_callbacks;

    // Multithreading
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_stopped{false};
    std::atomic<int> m_activeWriters{0};

};

}
