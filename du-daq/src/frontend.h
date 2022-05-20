/**
Created by duanbh,
2022/4/25
**/
#pragma once

#include "scope.h"
#include <thread>
#include <functional>

namespace grand {

typedef std::function<void(char *data, size_t sz)> ReadoutCallback;

class IFrontend{
public:
    IFrontend();
    virtual ~IFrontend();

    void setCallback(ReadoutCallback cb);

    // 状态转换函数
    void initialize();
    void configure(void *parameters);
    void start();
    void stop();
    void terminate();

protected:
    virtual void elecInit() = 0;
    virtual void elecConfig(void *parameters) = 0;
    virtual void elecStartRun() = 0;
    virtual int elecReadData(char *data, size_t maxSize) = 0;
    virtual void elecStopRun() = 0;
    virtual void elecTerminate() = 0;

private:
    bool m_stop;
    ReadoutCallback m_callback;

    std::thread m_thread;
    void inputThread();
};

}
