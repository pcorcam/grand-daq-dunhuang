/**
    *Created by duanbh,
    *2022.4.22,
    *Thits .h is for the exiting foreign software, because we now use the version to do some work, when we have ours then we could update all the program.
**/
#pragma once

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <functional>
#include <iostream>
#include <fstream>
#include <map>

#include "frontend.h"

#define DEVFILE "/dev/mem"
#define Reg_End 0x1FC

namespace grand {

class ScopeA: public IFrontend {
    public:
        ScopeA();
        ~ScopeA();

    protected:
        virtual void elecInit();
        virtual void elecConfig(void *parameters);
        virtual void elecStartRun();
        virtual int elecReadData(char *data, size_t maxSize);
        virtual void elecStopRun();
        virtual void elecTerminate();

    private:
        void scopeRawWrite(uint32_t regAddr, uint32_t value);
        int32_t scopeRawRead(uint32_t regAddr, uint32_t *value);
        void scopeSetParameter(uint32_t regAddr, uint32_t value, bool toShadow = false);
        void scopeFlush();

        uint32_t m_pageOffset;
        int32_t m_dev = 0;
        void *m_axiPtr;
        uint32_t m_shadowList[Reg_End>>2];
};

}
