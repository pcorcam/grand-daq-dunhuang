#include <iostream>
#include <stdio.h>
#include <thread>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <zmq_server.h>
#include <zmq.h>
#include <utils.h>
#include <chrono>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace grand;
int ddl;

        inline static uint64_t nowMilliSeconds() {
            std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
            return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
        }

        inline static uint64_t nowMicroSeconds() {
            std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> tp = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
            return std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch()).count();
        }

bool check(char *s1, size_t len) {
    for(size_t i=0; i<len; i++) {
        if(s1[i] != (char)i) {
            return false;
        }
    }
    return true;
}

void setBuffer(size_t sz, char *buf) {
    uint32_t id = 0;
    for(size_t i = 0; i < sz-4; i++) {
        memcpy(buf, &id, sizeof(uint32_t));
        buf[i+4] = (char)i;
        id++;
    }
}

void clientThread() {
    void *context = NULL;
    uint64_t t_start = nowMilliSeconds();
    uint64_t t_end = nowMilliSeconds();
    context = zmq_ctx_new();
    std::string clientID = "CSDAQ";

    char data[100000];
    char tmp[1];

    void *socket = zmq_socket(context, ZMQ_DEALER);
    int rc = zmq_setsockopt(socket, ZMQ_IDENTITY, clientID.c_str(), clientID.size());
    assert(rc == 0);

    std::string url = std::string("tcp://localhost:") + "19555";
    zmq_connect(socket, url.c_str());

    size_t testLen = 8720;
    // for(int i=0; i<10; i++) {
    while(true) {
        setBuffer(testLen, data);
        int rd = zmq_send(socket, "", sizeof(""), ZMQ_SNDMORE);
        rd = zmq_send(socket, data, testLen, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(8064));
        t_end = nowMilliSeconds();

        if(t_end - t_start > ddl*1000) {
            std::cout << "Send program runs for " << time << "s." << std::endl;
            break;
        }
    }

    rc = zmq_recv(socket, data, 100000, 0);
    rc = zmq_recv(socket, data, 100000, 0);
    //std::cout << rc << std::endl;
    assert(rc == 1024);
    assert(check(data, 1024));

    zmq_close(socket);
    zmq_ctx_destroy(context);
}

int main(int argc, char* argv[])
{
    LOG(INFO) << "test_backend start...";
    ddl = atoi(argv[1]);
    int recvCount = 0;
    std::thread csdaqThread;
    csdaqThread = std::thread(clientThread);
    ZMQServer *server = new ZMQServer;
    server->setup(102400, "tcp://*:19555", 128);
    server->setCallback([&recvCount](char *data, size_t sz) -> void {
        //std::cout << "msg recv" << std::endl;
        recvCount++;
        assert(check(data, sz));
    });
    server->initialize();
    usleep(10000);

    // wirte test
    char testData[1024];
    setBuffer(1024, testData);
    server->write(testData, 1024);
    //std::cout << "msg sent" << std::endl;

    csdaqThread.join();

    LOG(INFO) << "recv count = " << recvCount;
    // assert(recvCount == 10);

    server->terminate();
    return 0;
}
