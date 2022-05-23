#include <zmq_server.h>
#include <utils.h>

#define ZMQ_HAVE_POLLER
#define ZMQ_BUILD_DRAFT_API
#include <zmq.h>

using namespace grand;
using namespace std;

ZMQServer::ZMQServer() {
}

void ZMQServer::setup(int inputBufferSize, std::string backendBindUrl, int maxClientAddressSize)
{
    Server::setup(inputBufferSize);
    m_backendBindUrl = backendBindUrl;
    m_maxClientAddressSize = maxClientAddressSize;
}

ZMQServer::~ZMQServer() {
}

void ZMQServer::initialize() {
    m_address = new char[m_maxClientAddressSize];
    m_addressSize = 0;

    m_context = zmq_ctx_new();
    m_socket = zmq_socket(m_context, ZMQ_ROUTER);
    zmq_bind(m_socket, m_backendBindUrl.c_str());

    m_poller = zmq_poller_new();
    zmq_poller_add(m_poller, m_socket, nullptr, ZMQ_POLLIN);

    Server::initialize();
}

void ZMQServer::terminate() {
    Server::terminate();

    delete m_address;
    m_address = nullptr;
    m_addressSize = 0;
}

size_t ZMQServer::read(char* p, size_t maxSz) {

    int more;
    size_t more_size = sizeof(more);
    size_t recvSize = 0;
    zmq_poller_event_t events[1];
    int rc = zmq_poller_wait_all(m_poller, events, 1, 1000);
    bool succ = true;
    if(rc > 0) {
        assert(("", rc == 1));

        // recv address
        if(succ) {
            int nbytes = zmq_recv(m_socket, m_address, m_maxClientAddressSize, 0);
            if(nbytes < 0) {
                succ = false;
            }
            int rc1 = zmq_getsockopt(m_socket, ZMQ_RCVMORE, &more, &more_size);
            assert(rc1 == 0);
            assert(more);
            m_addressSize = nbytes;
            //std::cout << m_address << std::endl;
        }

        // recv delimiter
        if(succ) {
            int nbytes = zmq_recv(m_socket, p, maxSz, 0);
            if(nbytes < 0) {
                succ = false;
            }
            assert(nbytes == 1);
            int rc1 = zmq_getsockopt(m_socket, ZMQ_RCVMORE, &more, &more_size);
            assert(rc1 == 0);
            assert(more);
        }

        // recv data
        while(succ && more) {
            int nbytes = zmq_recv(m_socket, p+recvSize, maxSz-recvSize, 0);
            assert(nbytes <= maxSz-recvSize);
            if(nbytes < 0) {
                succ = false;
                break;
            }
            recvSize += nbytes;
            int rc1 = zmq_getsockopt(m_socket, ZMQ_RCVMORE, &more, &more_size);
            assert (rc1 == 0);
        }
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

void ZMQServer::write(char* p, size_t sz) {    
    assert(m_addressSize > 0);
    CLOG(DEBUG, "network") << "send message(fromt server), size = " << sz;
    int snd1 = zmq_send(m_socket, m_address, m_addressSize, ZMQ_SNDMORE);
    int snd2 = zmq_send(m_socket, "", sizeof(""), ZMQ_SNDMORE);
    int snd3 = zmq_send(m_socket, p, sz, 0);
}

