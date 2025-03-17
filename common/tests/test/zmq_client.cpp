#include <zmq_client.h>
#include <exceptions.h>
#include <utils.h>
#include <message_impl.h>

#define ZMQ_HAVE_POLLER
#define ZMQ_BUILD_DRAFT_API
#include <zmq.h>

using namespace std;
using namespace grand;

ZMQClient::ZMQClient() {
    m_inited = false;
}

ZMQClient::~ZMQClient() {
}

void ZMQClient::setup(int inputBufferSize, int zmqSndBufferSize) {
    Client::setup(inputBufferSize);
    m_zmqSndBufferSize = zmqSndBufferSize;
}

void ZMQClient::initialize() {
    if(m_inited) {
        throw NetworkError("double initialization of ZMQNetowrk");
    }

    m_context = zmq_ctx_new();
    // struct timeval timeout;
    // timeout.tv_usec = 100;
    // timeout.tv_sec = 0;
    double timeout = 0.1;
    for(auto &info: m_clientInfo) {
        ClientItem item;
        item.ID  = info.ID;
        item.socket = zmq_socket(m_context, ZMQ_DEALER);
        int rc = zmq_setsockopt(item.socket, ZMQ_IDENTITY, "CSDAQ", strlen("CSDAQ"));
        assert(rc == 0);
        rc = zmq_setsockopt(item.socket, ZMQ_SNDBUF, &m_zmqSndBufferSize, sizeof(m_zmqSndBufferSize));
        assert(rc == 0);
        int sndhwm = 1000;
        size_t sndhwm_size = sizeof(sndhwm);
        
        rc = zmq_setsockopt(item.socket, ZMQ_SNDHWM, &sndhwm, sndhwm_size);
        assert(rc == 0);
        rc = zmq_setsockopt(item.socket, ZMQ_RCVBUF, &m_zmqSndBufferSize, sizeof(m_zmqSndBufferSize));
        assert(rc == 0);
        // rc = zmq_setsockopt(item.socket, ZMQ_SNDTIMEO, &timeout, sizeof(double));
        // assert(rc < 0);
        // rc = zmq_setsockopt(item.socket, ZMQ_RCVTIMEO, &timeout, sizeof(double));
        // assert(rc < 0);

        std::string url = std::string("tcp://") + info.ip + ":" + std::to_string(info.port);
        CLOG(INFO, "network") << "connect socket: " << url;
        zmq_connect(item.socket, url.c_str());

        m_clientItem[item.ID] = item;
    }

    m_poller = zmq_poller_new();
    for(auto &item: m_clientItem) {
        zmq_poller_add(m_poller, item.second.socket, (void*)&item.second, ZMQ_POLLIN);
    }
        
    Client::initialize();

    char hshake[Message::messageHeaderSize];
    HandshakeMessage hs(hshake, Message::messageHeaderSize, true);
    writeAll(hs.base(), hs.size());

    m_inited = true;
}

void ZMQClient::terminate() {
    // std::cout << 111 << std::endl;
    Client::terminate();

    for(auto &item: m_clientItem) {
        zmq_close(item.second.socket);
    }
    m_clientItem.clear();
    zmq_ctx_destroy(m_context);
    // std::cout << 112 << std::endl;
    m_context = nullptr;

    m_inited = false;
}

//p needs to receive msg1 from Adaq, it's waiting for completing
size_t ZMQClient::read(char* p, size_t maxSz, std::string &ID) {
    int more;
    size_t more_size = sizeof(more);
    size_t recvSize = 0;
    zmq_poller_event_t events[1];
    int rc = zmq_poller_wait_all(m_poller, events, 1, 100);
    //// std::cout << "rc: " << rc << std::endl;
    bool succ = true;
    if(rc > 0) {
        assert(("", rc == 1));
        void *socket = events[0].socket;
        ClientItem *client = (ClientItem*)(events[0].user_data);
        ID = client->ID;

        // recv delimiter
        {
            int nbytes = zmq_recv(socket, p, maxSz, 0);
            if(nbytes < 0) {
                succ = false;
            }
            assert(nbytes == 1);
            int rc1 = zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &more_size);
            assert(rc1 == 0);
            assert(more);
        }

        // recv data
        do {
            int nbytes = zmq_recv(socket, p+recvSize, maxSz-recvSize, 0);
            assert(nbytes <= maxSz-recvSize);
            if(nbytes < 0) {
                succ = false;
                break;
            }
            recvSize += nbytes;
            int rc1 = zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &more_size);
            assert (rc1 == 0);
        } while(more);
    }
    else {
        succ = false;
    }

    if(succ) {
        return recvSize;
    }
    else {
        return 0;
    }
}

void ZMQClient::write(std::string ID, char* p, size_t sz) {    
    // std::cout << 4564 << std::endl;
    if(m_clientItem.count(ID) == 0) {
        throw NetworkError("socket does not connect, used in read function");
    }
    // std::cout << 66787 << std::endl;
    void *socket = m_clientItem[ID].socket;
    // std::cout << 687613 << std::endl;
    int rc = zmq_send(socket, "", sizeof(""), ZMQ_SNDMORE);
    // std::cout << 9856 << std::endl;
    assert(("", rc == 1));
    // rc = zmq_send(socket, ID.c_str(), ID.length(), ZMQ_SNDMORE);
    // assert(("", rc == ID.length())); // ***
    // std::cout << 54687 << std::endl;
    rc = zmq_send(socket, p, sz, 0);
    assert(("", rc == sz));    
}
