#include <csdaq_app.h>
#include <dudaq_app.h>
#include <utils.h>

using namespace grand;

int main(int argc, char **argv)
{
    LOG(INFO) << "DU DAQ is runing..";

    grand::DUDAQApp duApp;
    grand::CSDAQApp csApp;

    duApp.sysInit();
    csApp.sysInit();

    csApp.startDAQ();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    csApp.stopDAQ();

    csApp.sysTerm();
    duApp.sysTerm();
    return 0;
}
