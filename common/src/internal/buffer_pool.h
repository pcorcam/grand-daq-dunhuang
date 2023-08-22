#pragma once

#include <list>
#include <mutex>
#include <condition_variable>

namespace grand {

class BufferPool {
public:

    BufferPool(size_t bufSize, int numBuffers);

    char *request(int timeoutMs);
    void release(char *);

private:
    int m_numBuffers;
    std::list<char*> m_buffers;

    std::mutex m_mutex;
    std::condition_variable m_cond;
};

}
