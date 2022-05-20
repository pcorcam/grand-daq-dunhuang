#pragma once

#include <cstdio>
#include <iostream>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string.h>
#include <types.h>

namespace grand {

const MessageType MT_HSHAKE = 0x10FF;
const MessageType MT_NONE = 0x00;
const MessageType MT_CMD = 0x01;
const MessageType MT_ACCEPT = 0x02;
const MessageType MT_T2 = 0x03;
const MessageType MT_DAQEVENT = 0x04;

struct MessageHeader {
    uint32_t size;
    MessageType type;
};

class Message 
{
public:
    const static uint32_t messageHeaderSize = sizeof(MessageHeader);
    Message(char *data, size_t sz, bool write = false, MessageType type = MT_NONE);
    ~Message();
    char *base();
    MessageHeader* header();
    MessageType type();
    uint32_t size();
    uint32_t dataSize(); 
    char* data();
    void sizeInc(size_t sz);
    void setSizeByData(size_t sz);

protected:
    char* m_data;
    size_t m_size;
};

}
