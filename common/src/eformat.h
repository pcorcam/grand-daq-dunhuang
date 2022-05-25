#pragma once

namespace grand {

#define DAQPCK_TYPE_DUEVENT 0x01
#define DAQPCK_TYPE_T3EVENT 0x02

struct __attribute__((__packed__)) DAQHeader {
    uint32_t size;
    uint32_t type;
    uint32_t source;
};

}
