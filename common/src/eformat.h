#pragma once

namespace grand {

#define DAQPCK_TYPE_DUEVENT 0x01
#define DAQPCK_TYPE_T2EVENT 0x02
#define DAQPCK_TYPE_T3EVENT 0x03

struct __attribute__((__packed__)) DAQHeader {
    uint32_t size;
    uint32_t type;
    uint32_t source;
};

struct __attribute__((__packed__)) T2Header {
    uint32_t size;
    uint32_t type;
    uint32_t source;
};

struct __attribute__((__packed__)) RawEventHeader {
    uint32_t size;
    uint32_t type;
    uint32_t source;
};

}
