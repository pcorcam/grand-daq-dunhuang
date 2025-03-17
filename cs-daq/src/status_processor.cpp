#include <status_processor.h>
#include <utils.h>

using namespace grand;

StatusProcessor::StatusProcessor() {
}

void StatusProcessor::processData(std::string du, char *data, size_t sz) {
    // TODO: this is dummy
    CLOG(INFO, "data") << "input status from DU = " << du
            << ", datasize = " << sz;
}
