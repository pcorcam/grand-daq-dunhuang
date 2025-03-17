#include <server.h>
#include <utils.h>

using namespace grand;

Server::Server() { 
    m_callback = nullptr;
}

Server::~Server() {
    if(m_thread) {
        terminate();
    }
}

void Server::setup(int inputBufferSize)
{
    m_inputBufferSize = inputBufferSize;
}

void Server::initialize() {
    // // std::cout << "Server::initialize" << std::endl;
    CLOG(INFO, "network") << "Server, input buffer size = " << m_inputBufferSize;
    m_stop = false;
    m_thread = new std::thread(&Server::inputThread,this);
    // // std::cout << "Server::initialize end" << std::endl;
}

void Server::terminate() {
    // // std::cout << "Server::terminate" << std::endl;
    if (m_thread) {
        m_stop = true;
        m_callback = nullptr;  
        m_thread->join();
        delete m_thread;
        m_thread = nullptr;
    }
    // // std::cout << "Server::terminate end" << std::endl;
}

void Server::setCallback(ServerInputCallback cb) {
    m_callback = cb;
}

void Server::inputThread() {
    // std::cout << "Server::inputThread" << std::endl;
    CLOG(INFO, "network") << "Server, input thread started";
    char* cmd = new char[m_inputBufferSize];
    
    while(true) {
        if(m_stop) {
            break;
        }
        size_t sz = read(cmd, m_inputBufferSize);
        if(sz > 0) { 
            //// std::cout << "DU input" << std::endl;
            if(m_callback) {
                m_callback(cmd, sz);
            }
        }
        else {
            //// std::cout << "no msg is recvd" << std::endl;
        }
   }
    delete[] cmd;
    cmd = nullptr;
    // std::cout << "Server::inputThread end" << std::endl;
}
