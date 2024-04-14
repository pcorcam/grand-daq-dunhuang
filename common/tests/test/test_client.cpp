#include <zmq_client.h>
#include <zmq.h>
#include <utils.h>

bool stop = false;

using namespace grand;

#define DATA_SIZE 1024

void serverThread() {
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_ROUTER);
    std::string url = "tcp://*:19555";
    CLOG(INFO, "network") << "bind socket: " << url;
    zmq_bind(socket, url.c_str());

    char *data = new char[DATA_SIZE];
    for(int i=0; i<DATA_SIZE; i++) {
        data[i] = (char)i;
    }

    char addr[100];
    size_t addrLen = 0;
    char cmd[100];
    addrLen = zmq_recv(socket, addr, 100, 0);
    zmq_recv(socket, cmd, 100, 0);

    while(!stop) {
        //std::cout << "1" << std::endl;
        zmq_send(socket, addr, addrLen, ZMQ_SNDMORE);
        zmq_send(socket, "", sizeof(""), ZMQ_SNDMORE);
        zmq_send(socket, data, DATA_SIZE, 0);
        //std::cout << "2" << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main()
{
    ZMQClient cli;
    cli.addCallback([](std::string ID, char *data, size_t sz) -> void {
        CLOG(DEBUG, "network") << "RECV from ID = " << ID << ", size = " << sz;
        assert(sz == DATA_SIZE);
        for(int i=0; i<sz; i++) {
            //std::cout << (int)data[i] << std::endl;
            assert(data[i] == (char)i);
        }
    });

    cli.setup(102400, 2048);
    cli.addClient("DU1", "localhost", 19555);
    cli.initialize();

    std::thread th(serverThread);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    stop = true;
    th.join();

    cli.terminate();
}
