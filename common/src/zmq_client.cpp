#include <zmq_client.h>
#include <exceptions.h>
#include <utils.h>
#include <message_impl.h>
#include <cs_sys_config.h>

#define ZMQ_HAVE_POLLER
#define ZMQ_BUILD_DRAFT_API
#include <zmq.h>

// extern int csDAQMODE;

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
        // int sndhwm = 1000;
        // size_t sndhwm_size = sizeof(sndhwm);
        
        // rc = zmq_setsockopt(item.socket, ZMQ_SNDHWM, &sndhwm, sndhwm_size);
        // assert(rc == 0);
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
    Client::terminate();

    for(auto &item: m_clientItem) {
        zmq_close(item.second.socket);
    }
    m_clientItem.clear();
    zmq_ctx_destroy(m_context);
    m_context = nullptr;

    m_inited = false;
}

//p needs to receive msg1 from Adaq, it's waiting for completing
size_t ZMQClient::read(char* p, size_t maxSz, std::string &ID) {
    // std::cout << "ZMQClient::read" << std::endl;
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
            // // std::cout << " nbytes is " << nbytes << std::endl;
            if(nbytes != 1) {
                // // std::cout << " nbytes is " << nbytes << std::endl;
                return 0;
            }
            // assert(nbytes == 1);
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
    // std::cout << "ZMQClient::read end" << std::endl;
    if(succ) {
        return recvSize;
    }
    else {
        return 0;
    }
}

void ZMQClient::write(std::string ID, char* p, size_t sz) { 
    // std::cout << "ZMQClient::write" << std::endl;
    // std::unique_lock<mutex> lock(m_mutex);
    // std::cout << "ZMQClient::write 1" << std::endl;
    m_csDAQMODE = CSSysConfig::instance()->appConfig().daqMode;
    // std::cout << "ZMQClient::write 2" << std::endl;
    int daqMode = m_csDAQMODE;
    sprintf(m_daqMode, "%d", daqMode);
    // std::cout << "ZMQClient::write 3" << std::endl;

    if(m_clientItem.count(ID) == 0) {
        // std::cout << "ZMQClient::write 4" << std::endl;
        throw NetworkError("socket does not connect, used in read function");
        // std::cout << "ZMQClient::write 5" << std::endl;
    }
    void *socket = m_clientItem[ID].socket;
    if (!socket) {
        std::cerr << "ZMQClient::write error: socket is null!" << std::endl;
        throw std::runtime_error("Socket is null");
    }
    // ************************ test area ************************* //
    // int hwm;
    // size_t hwm_size = sizeof(hwm);
    // zmq_getsockopt(socket, ZMQ_SNDHWM, &hwm, &hwm_size);
    // // std::cout << "ZMQ_SNDHWM: " << hwm << std::endl;
    int hwm = 10000;
    zmq_setsockopt(socket, ZMQ_SNDHWM, &hwm, sizeof(hwm));
    int events;
    size_t events_len = sizeof(events);
    int rc = zmq_getsockopt(socket, ZMQ_EVENTS, &events, &events_len);
    if(rc == -1) {
        // std::cout << "rc == -1" << std::endl;
        int err = zmq_errno();
        if (err == EAGAIN || err == ETERM || err == ECONNRESET) {
            CLOG(WARNING, "network") << "Client " << ID << " disconnected, attempting to reconnect...";
            // std::cout << "ZMQClient::write 3.1" << std::endl;
            zmq_close(socket);
            // std::cout << "ZMQClient::write 3.2" << std::endl;
            m_clientItem.erase(ID);
            // std::cout << "ZMQClient::write 3.3" << std::endl;
            reconnect(ID);  // ** need a try to connect**
            // std::cout << "ZMQClient::write 3.3" << std::endl;
            return;
        }
    }
    // std::cout << "ZMQ_EVENTS: " << events << std::endl;  
    if (!(events & ZMQ_POLLOUT)) {
        std::cerr << "DU: "<< ID << ", ZMQClient::write error: socket not ready for writing!" << std::endl;
        return;
    }

    // ************************ test area ************************* //

    // std::cout << "ZMQClient::write 6" << std::endl;
    // int rc = zmq_send(socket, "", 0, ZMQ_SNDMORE | ZMQ_DONTWAIT);
    // if (rc == -1) {
    //     // std::cout << "zmq_send error: " << zmq_strerror(zmq_errno()) << std::endl;
    // }
    rc = zmq_send(socket, "", sizeof(""), ZMQ_SNDMORE);
    // std::cout << "ZMQClient::write 7" << std::endl;
    assert(("", rc == 1));
    // std::cout << "ZMQClient::write 8" << std::endl;
    rc = zmq_send(socket, m_daqMode, sizeof(m_daqMode), ZMQ_SNDMORE);
    // std::cout << "ZMQClient::write 9" << std::endl;
    // // std::cout << "we need send rc's value is " << rc << std::endl;
    rc = zmq_send(socket, p, sz, 0);
    // std::cout << "ZMQClient::write 10" << std::endl;
    assert(("", rc == sz));    
    // std::cout << "ZMQClient::write end" << std::endl;
}

void ZMQClient::reconnect(std::string ID) {
    std::this_thread::sleep_for(std::chrono::seconds(1));  // **等待 1s，避免过快重试**
    
    void* new_socket = zmq_socket(m_context, ZMQ_DEALER);  
    if (!new_socket) {
        std::cerr << "Failed to create new socket: " << zmq_strerror(errno) << std::endl;
        return;
    }
    std::string url = std::string("tcp://192.168.61.") + ID + ":19555";
    if (zmq_connect(new_socket, url.c_str()) != 0) {
        std::cerr << "Failed to reconnect: " << zmq_strerror(errno) << std::endl;
        zmq_close(new_socket);
        return;
    }

    m_clientItem[ID].socket = new_socket;
    // std::cout << "Reconnected to " << m_clientItem[ID].socket<< std::endl;
}

