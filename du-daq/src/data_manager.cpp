#include <data_manager.h>

using namespace grand;

DataManager::DataManager() {}

void DataManager::setEventOutput(EventOutput fun) {
    m_eventOutputFun = fun;
}

void DataManager::initialize()
{
    // TODO: create event buffer
}

void DataManager::terminate()
{
    // TODO: release all
}

void DataManager::addEvent(char *data, size_t sz)
{
    // FIXME: currently, directly send event data to cs-daq
    if(m_eventOutputFun) {
        m_eventOutputFun(data, sz);
    }
}

void DataManager::accept(char *data, size_t sz)
{

}

void DataManager::generateT2(char *data, size_t sz)
{

}

