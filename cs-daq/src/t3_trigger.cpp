#include <t3_trigger.h>
#include <utils.h>
#include <cs_sys_config.h>
#include <cassert>

using namespace grand;

T3Trigger::T3Trigger(TriggerDone triggerDoneCallback) {
    m_bufferSize = CSSysConfig::instance()->appConfig().t2BufferPageSize;
    int numPages = CSSysConfig::instance()->appConfig().t2BufferNumberOfPages;
    m_buffers = new BufferPool(m_bufferSize, numPages);
    m_threadPool = new ThreadPool(1);
    m_triggerDone = triggerDoneCallback;
}

T3Trigger::~T3Trigger() {
    delete m_buffers;
}

void T3Trigger::processData(std::string du, char *data, size_t sz) {
    // TODO: this is dummy
    CLOG(INFO, "data") << "input T2 from DU = " << du
            << ", datasize = " << sz;
    CLOG(WARNING, "data") << "T3 trigger is not implemented";

    assert(m_bufferSize >= sz);
    // char *ptr = nullptr;
    // while(ptr == nullptr) {
    //     ptr = m_buffers.request(100);
    // }
    // memcpy(ptr, data, sz);
    //threadPool.submit(std::bind(&T3Trigger::doTrigger, this, m_triggerDone));
}

void T3Trigger::doTrigger(TriggerDone cb) {
    if(cb) {
        cb();
    }
}

