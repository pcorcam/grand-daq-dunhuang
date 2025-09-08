#include <client.h>
#include <utils.h>

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
    CLOG(INFO, "network") << "Client, input buffer size = " << m_inputBufferSize;
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
        // m_t2callbacks.clear();
        m_clientInfo.clear();
        delete m_buffer;
        m_buffer = nullptr;
    }
};

void Client::waitForWriteAllCompletion() {
    // std::cout << "Wainting for WriteAll" << std::endl;
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [this] {
        return m_activeWriters.load() == 0;
    });
    // std::cout << "Finished waiting for WriteAll" << std::endl;
}

void Client::writeAll(char *p, size_t sz) {
    m_activeWriters.fetch_add(1);
    // std::string tmp(p, 1000);
    // std::cout << "tmp str " << tmp << std::endl;


    // std::cout << "sz of data input writeAll is " << sz << std::endl;
    // std::cout << "Client::writeAll" << std::endl;
    for(auto &info: m_clientInfo) {
        // std::cout << "duID: " << info.ID << std::endl;
        this->write(info.ID, p, sz);
    }
    // std::cout << "Client::writeAll end" << std::endl;
    m_activeWriters.fetch_sub(1);
    m_cv.notify_one();
}

void Client::writeOne(std::string ID, char *p, size_t sz) {
    // std::cout << "this is writeone func" << std::endl;
    // std::cout << "sz of data input writeAll is " << sz << std::endl;
    // std::cout << "duID is " << ID << std::endl;
    // this->write(ID, p, sz);
    write(ID, p, sz);
    // std::cout << "That's all for writeOne" << std::endl;
}

Client::Client() {
}

Client::~Client() {
    terminate();
}

void Client::inputThread() {
    CLOG(INFO, "network") << "Client, input thread started";
    while(!m_stop) {
        std::string ID;
        size_t sz = this->read(m_buffer, m_inputBufferSize, ID);
        // size_t sz_t2=this->read(m_t2buffer,m_t2inputBufferSize, ID);
        //std::cout << sz << std::endl;
        if(sz > 0) {
            CLOG(DEBUG, "network") << "Client input, size = " << sz;
            for(auto cb: m_callbacks) {
                cb(ID, m_buffer, sz);
            }
        }
    }
}
