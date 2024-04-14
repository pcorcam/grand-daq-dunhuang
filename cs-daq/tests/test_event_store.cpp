#include <event_store.h>
#include <utils.h>

using namespace grand;

#define DATA_ELEMENT_SIZE 1500
#define WRITE_TIMES 200

int main() {
    LOG(INFO) << "start writing 1500*200 bytes to file stream, max size of each file = " << 100*1024; 
    ESFileHeaderWriter fh;
    EventStore es("GRAND_EVENT", "TEST", "", 100*1024, &fh, false);

    char buf[DATA_ELEMENT_SIZE];
    for(int i=0; i<DATA_ELEMENT_SIZE; i++) {
        buf[i] = (char)(uint32_t)i;
    }

    es.openStream();
    for(int i=0; i<WRITE_TIMES; i++) {
        es.write(buf, DATA_ELEMENT_SIZE);
    }
    es.closeStream();
}
