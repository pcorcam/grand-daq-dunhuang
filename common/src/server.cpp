#include <server.h>

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
    m_stop = false;
    m_thread = new std::thread(&Server::inputThread,this);
}

void Server::terminate() {
    m_stop = true;
    m_thread->join();
    delete m_thread;
    m_thread = nullptr;
}

void Server::setCallback(ServerInputCallback cb) {
    m_callback = cb;
}

void Server::inputThread() {
    char* cmd = new char[m_inputBufferSize];
    
    while(true) {
        if(m_stop) {
            break;
        }
        size_t sz = read(cmd, m_inputBufferSize);
        if(sz > 0) {       
            if(m_callback) {
                m_callback(cmd, sz);
            }
        }
        else {
            //std::cout << "no msg is recvd" << std::endl;
        }
   }
}
