/**
 ** Created by duanbh,
 ** 2022/4/25
 **/
#include <iostream>
#include "frontend.h"
#include "sys_config.h"

using namespace grand;

void IFrontend::inputThread() {
    size_t bufSize = SysConfig::instance()->readoutBufferSize;
    char *buf = new char[bufSize];
    while(true) {
        if(m_stop) {
            break;
        }
        int retSz = elecReadData(buf, bufSize);
        if(retSz > 0) {
            if(m_callback) {
                m_callback(buf, retSz);
            }
        }
    }
    delete buf;
}

IFrontend::IFrontend() : m_callback(nullptr) {
}

IFrontend::~IFrontend() {
}

void IFrontend::setCallback(ReadoutCallback cb) {
    m_callback = cb;
}

void IFrontend::initialize() {
    elecInit();
}

void IFrontend::configure(void *parameters) {
    elecConfig(parameters);
}

void IFrontend::start(){
    elecStartRun();

    m_stop = false;
    m_thread = std::thread(&IFrontend::inputThread,this);
    elecStartRun();
}

void IFrontend::stop(){
    m_stop = true;
    m_thread.join();

    elecStopRun();
}

void IFrontend::terminate() {
    elecTerminate();
}

