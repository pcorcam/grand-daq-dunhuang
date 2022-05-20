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

        uint32_t page_offset;
        int32_t dev = 0;
        void *axi_ptr;
        uint32_t shadowlist[Reg_End>>2];
        uint16_t evtlen;
        uint16_t *evtbuf=NULL;                                          
    protected:
        virtual void elecInit();
        virtual void elecConfig(void *parameters);
        virtual void elecStartRun();
        virtual int elecReadData(char *data, size_t maxSize);
        virtual void elecStopRun();
        virtual void elecTerminate();

    private:
        void scope_raw_write(uint32_t reg_addr, uint32_t value);
        int32_t scope_raw_read(uint32_t reg_addr, uint32_t *value);
        void scope_set_parameters(uint32_t reg_addr, uint32_t value,uint32_t to_shadow);
        void scope_flush();
};

}
