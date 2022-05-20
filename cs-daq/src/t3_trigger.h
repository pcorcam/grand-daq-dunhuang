#pragma once

#include <string>
#include <utils.h>

namespace grand {

typedef std::function<void ()> TriggerDone;

class T3Trigger {
public:
    T3Trigger(TriggerDone triggerDoneCallback = nullptr);
    ~T3Trigger();

    void processData(std::string du, char *data, size_t sz);

private:
    BufferPool *m_buffers;
    int m_bufferSize;

    ThreadPool *m_threadPool;
    TriggerDone m_triggerDone;

    void doTrigger(TriggerDone cb);
};

}
