#include <dudaq_app.h>
#include <utils.h>

int main(int argc, char **argv)
{
    LOG(INFO) << "DU DAQ is runing..";

    grand::DUDAQApp app;
    app.sysInit();
    while(true) {
        sleep(1);
    }
    return 0;
}
