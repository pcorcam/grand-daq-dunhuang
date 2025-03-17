#include <iostream>
#include <stdio.h>
#include <thread>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <zmq_server.h>
#include <zmq_client.h>
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

int main()
{
    LOG(INFO) << "test_communication start...";
    int sRecv = 0;
    int cRecv = 0;

    // initialize client (csdaq)
    ZMQClient cli;
    cli.addCallback([&cRecv](std::string ID, char *data, size_t sz) -> void {
        CLOG(DEBUG, "network") << "Client: RECV from ID = " << ID << ", size = " << sz;
        cRecv++;
        assert(check(data, sz));
    });

    cli.setup(102400, 2048);
    cli.addClient("DU1", "localhost", 19555);
    cli.initialize();

    // initialize server (dudaq)
    ZMQServer ser;
    ser.setup(102400, "tcp://*:19555", 128);
    ser.setCallback([&sRecv](char *data, size_t sz) -> void {
        CLOG(DEBUG, "network") << "Server: RECV size = " << sz;
        if(sRecv == 0) {
            // skip first handshake message
            return;
        }
        sRecv++;
        assert(check(data, sz));
    });
    ser.initialize();

    char testData[1024];
    setBuffer(1024, testData);

    sleep(1);

    // client wirte test
    cli.writeAll(testData, 1024);

    // server wirte test
    ser.write(testData, 1024);
 
    usleep(10000);

    LOG(INFO) << "Server: recv count = " << sRecv;
    LOG(INFO) << "Client: recv count = " << cRecv;

    cli.terminate();
    ser.terminate();
    return 0;
}
