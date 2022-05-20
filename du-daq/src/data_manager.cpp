#include <data_manager.h>
#include <cassert>
#include <message_impl.h>

using namespace grand;

DataManager::DataManager() {}

void DataManager::setEventOutput(EventOutput fun) {
    m_eventOutputFun = fun;
}

void DataManager::initialize()
{
    // TODO: create event buffer
    m_eventBuffer = new char[1024000];
}

void DataManager::terminate()
{
    // TODO: release all
    delete m_eventBuffer;
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

