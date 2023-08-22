#include <internal/buffer_pool.h>

using namespace grand;

BufferPool::BufferPool(size_t bufSize, int numBuffers) {
    m_numBuffers = numBuffers;
    for(int i=0; i<numBuffers; i++) {
        m_buffers.push_back(new char[bufSize]);
    }
}

char *BufferPool::request(int timeoutMs) {
    std::unique_lock<std::mutex> lock(m_mutex);
    bool ret = m_cond.wait_for(lock, std::chrono::milliseconds(timeoutMs), [&]()->bool{ 
        return !m_buffers.empty();
    }) ;
    if(ret) {
        char *buffer = m_buffers.front();
        m_buffers.pop_back();
        return buffer;
    }
    else {
        return nullptr;
    }
}

void BufferPool::release(char *buffer) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_buffers.push_back(buffer);
    m_cond.notify_one();
}

