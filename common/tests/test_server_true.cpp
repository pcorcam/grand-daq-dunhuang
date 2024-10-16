#include <iostream>
#include <stdio.h>
#include <thread>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <zmq_server.h>
#include <zmq.h>
#include <utils.h>

using namespace grand;

bool check(char *s1, size_t len) {
    for(size_t i=0; i<len; i++) {
        if(s1[i] != (char)i) {
            return false;
        }
    }
    return true;
}

void setBuffer(size_t sz, char *buf) {
    for(size_t i = 0; i < sz; i++) {
        buf[i] = (char)i;
    }
}

void clientThread() {
    void *context = NULL;
    context = zmq_ctx_new();
    std::string clientID = "CSDAQ";

    char data[100000];
    char tmp[1];

    void *socket = zmq_socket(context, ZMQ_DEALER);
    int rc = zmq_setsockopt(socket, ZMQ_IDENTITY, clientID.c_str(), clientID.size());
    assert(rc == 0);

    std::string url = std::string("tcp://localhost:") + "19555";
    zmq_connect(socket, url.c_str());

    size_t testLen = 1000;
    for(int i=0; i<10; i++) {
        setBuffer(testLen, data);
        int rd = zmq_send(socket, "", sizeof(""), ZMQ_SNDMORE);
        rd = zmq_send(socket, data, testLen, 0);
    }

    rc = zmq_recv(socket, data, 100000, 0);
    rc = zmq_recv(socket, data, 100000, 0);
    //std::cout << rc << std::endl;
    assert(rc == 1024);
    assert(check(data, 1024));

    zmq_close(socket);
    zmq_ctx_destroy(context);
}

int main()
{
    LOG(INFO) << "test_backend start...";
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
    assert(recvCount == 10);

    server->terminate();
    return 0;
}
