#include <iostream>
#include <iomanip>
#include <cassert>
#include <message_impl.h>
#include <utils.h>

using namespace std;
using namespace grand;

#define MSG_SIZE 1024

int main() {
    LOG(INFO) << "test_message starts ..";
    char data[MSG_SIZE];
    {
        CommandMessage msg(data, MSG_SIZE, true);
        msg.setCmd("hello");
        assert(msg.cmd() == "hello");

        //std::cout << msg.header()->size << ", " << Message::messageHeaderSize + sizeof("hello");
        assert(msg.header()->size == Message::messageHeaderSize + sizeof("hello"));
        assert(msg.header()->type == MT_CMD);
    }

    {
        T2Message msg(data, MSG_SIZE, true);
        msg.addTQ(1231, 4561);
        msg.addTQ(1232, 4562);
        std::vector<T2Message::TQ> tqs = msg.TQData();
        assert(tqs.size() == 2);
        //std::cout << tqs[0].time << std::endl;
        assert(tqs[0].time == 1231);
        assert(tqs[1].charge == 4562);

        assert(msg.header()->size == Message::messageHeaderSize + sizeof(T2Message::TQ)*2);
        assert(msg.header()->type == MT_T2);
    }

    {
        AcceptMessage msg(data, MSG_SIZE, true);
        msg.addEventID(333);
        msg.addEventID(334);
        std::set<uint32_t> IDs = msg.eventIDs();
        assert(IDs.size() == 2);
        assert(IDs.count(333) == 1);
        assert(IDs.count(334) == 1);
        assert(msg.header()->size == Message::messageHeaderSize + sizeof(uint32_t)*2);
        assert(msg.header()->type == MT_ACCEPT);
    }

    {
        DAQEvent msg(data, MSG_SIZE, true);
        assert(msg.header()->size == Message::messageHeaderSize);
        assert(msg.header()->type == MT_DAQEVENT);

        char data1[100];
        msg.copyFrom(data1, 100);
        assert(msg.header()->size == Message::messageHeaderSize+100);
    }
    return 0;
}
