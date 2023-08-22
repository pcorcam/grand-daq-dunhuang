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
#include <message_impl.h>

using namespace grand;

int main()
{
    LOG(INFO) << "test_send_recv_cmd start...";
    int sRecv = 0;
    int cRecv = 0;

    // initialize client (csdaq)
    ZMQClient cli;
    cli.addCallback([&cRecv](std::string ID, char *data, size_t sz) -> void {
        CLOG(DEBUG, "network") << "Client: RECV from ID = " << ID << ", size = " << sz;
        cRecv++;
    });

    cli.setup(102400, 2048);
    cli.addClient("DU1", "localhost", 19555);
    cli.initialize();

    // initialize server (dudaq)
    ZMQServer ser;
    ser.setup(102400, "tcp://*:19555", 128);
    ser.setCallback([&sRecv](char *data, size_t sz) -> void {
        CLOG(DEBUG, "network") << "Server: RECV size = " << sz;
        Message msg(data, sz);
        if(msg.type() == MT_HSHAKE) {
            CLOG(DEBUG, "network") << "Server: RECV handshake message";
            return;
        }
        assert(msg.type() == MT_CMD);
        CommandMessage msg1(data, sz);
        assert(msg1.cmd() == "CONFIG");
        assert(msg1.paramSize() == 10);
        for(int i=0; i<msg1.paramSize(); i++) {
            //std::cout << (unsigned int)((unsigned char*)msg1.param())[i] << std::endl;
            assert(((char*)msg1.param())[i] == '\x01');
        }
        sRecv++;
    });
    ser.initialize();

    usleep(10000);

    // create command
    char testData[1024];
    char param[10];
    memset(param, 1, 10);
    CommandMessage cmdMsg(testData, 1024, true);
    cmdMsg.setCmd("CONFIG", param, 10);
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
