#include "message.h"

using namespace grand;

Message::Message(char *data, size_t sz, bool write, MessageType type) : m_data(data), m_size(sz) {
    if(write) {
        header()->type = type;
        header()->size = messageHeaderSize;
    }
}

Message::~Message() {
}

char *Message::base() {
    return m_data;
}

MessageHeader* Message::header() {
    return (MessageHeader*)m_data;
}

MessageType Message::type() { 
    return header()->type;
}

uint32_t Message::size() {
    return header()->size;
}

uint32_t Message::dataSize() {
    return size() - messageHeaderSize;
} 

char* Message::data() {
    return m_data+messageHeaderSize;
}

void Message::sizeInc(size_t sz) {
    header()->size = header()->size + sz;
}

void Message::setSizeByData(size_t sz) {
    header()->size = sz+messageHeaderSize;
}
