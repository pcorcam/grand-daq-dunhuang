#include <utils.h>
#include <cassert>

using namespace grand;

std::mutex mutex;
std::list<char *> inUse;

XEvent ev;

void testThread(BufferPool &bp) {
    
    for(int i=0; i<10; i++) {
        char *b = bp.request(10);
        assert(b != nullptr);
        std::lock_guard<std::mutex> lock(mutex);
        inUse.push_back(b);
    }

    {
        char *b = bp.request(10);
        assert(b == nullptr);
    }

    ev.wait(1000);

    {
        std::lock_guard<std::mutex> lock(mutex);
        char *b = bp.request(10);
        assert(b != nullptr);
    }
}

int main()
{
    LOG(INFO) << "start test BufferPool..";
    BufferPool bp(1024, 10);
    std::thread th(std::bind(testThread, std::ref(bp)));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::lock_guard<std::mutex> lock(mutex);
        assert(!inUse.empty());
        char *b = inUse.front();
        inUse.pop_front();
        bp.release(b);
    }

    th.join();

    return 0;
}
