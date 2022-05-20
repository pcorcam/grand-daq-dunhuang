#include <data_manager.h>
#include <cassert>
#include <message_impl.h>

using namespace grand;

DataManager::DataManager() : m_eventBuffer(nullptr) {}
DataManager::~DataManager() {
    terminate();
}

void DataManager::setEventOutput(EventOutput fun) {
    m_eventOutputFun = fun;
}

void DataManager::initialize()
{
    // TODO: create event buffer
    if(m_eventBuffer == nullptr) {
        m_eventBuffer = new char[1024000];
    }
}

void DataManager::terminate()
{
    // TODO: release all
    if(m_eventBuffer) {
        delete m_eventBuffer;
        m_eventBuffer = nullptr;
    }
}

void DataManager::addEvent(char *data, size_t sz)
{
    // FIXME: currently, directly send event data to cs-daq
    DAQEvent msg(m_eventBuffer, 1024000, true);
    msg.copyFrom(data, sz);
    if(m_eventOutputFun) {
        m_eventOutputFun(msg.base(), msg.size());
    }
}

void DataManager::accept(char *data, size_t sz)
{

}

void DataManager::generateT2(char *data, size_t sz)
{

}

