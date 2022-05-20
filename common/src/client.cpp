#include <client.h>

using namespace grand;

void Client::addClient(std::string id ,std::string ip, uint32_t port) {
    ClientInfo i;
    i.ID = id;
    i.ip = ip;
    i.port = port;
    m_clientInfo.push_back(i);
}

void Client::addCallback(ClientInputCallback cb) {
    m_callbacks.push_back(cb);
}

void Client::setup(int inputBufferSize)
{
    m_inputBufferSize = inputBufferSize;
}

void Client::initialize() {
    m_buffer = new char[m_inputBufferSize];
    m_stop = false;
    m_thread = new std::thread(std::bind(&Client::inputThread, this));
};

void Client::terminate() {
    if(!m_stop) {
        m_stop = true;
        m_thread->join();
        delete m_thread;
        m_thread = nullptr;

        m_callbacks.clear();
        m_clientInfo.clear();
        delete m_buffer;
        m_buffer = nullptr;
    }
};

void Client::writeAll(char *p, size_t sz) {
    //std::cout << sz << std::endl;
    for(auto &info: m_clientInfo) {
        this->write(info.ID, p, sz);
    }
}

void Client::writeOne(std::string ID, char *p, size_t sz) {
    this->write(ID, p, sz);
}

Client::Client() {
}

Client::~Client() {
    terminate();
}

void Client::inputThread() {
    while(!m_stop) {
        std::string ID;
        size_t sz = this->read(m_buffer, m_inputBufferSize, ID);
        //std::cout << sz << std::endl;
        if(sz > 0) {
            for(auto cb: m_callbacks) {
                //std::cout << "call callback" << std::endl;
                cb(ID, m_buffer, sz);
            }
        }
    }
}
