#include <csdaq_app.h>
#include <utils.h>

int main(int argc, char **argv)
{
    long runTime = 10;
    if(argc >= 2) {
        runTime = strtol(argv[1], nullptr, 0);
    }

    LOG(INFO) << "DAQ will run " << runTime << " seconds..";

    grand::CSDAQApp app;
    app.sysInit();
    app.startDAQ();

    std::this_thread::sleep_for(std::chrono::milliseconds(runTime * 1000));

    app.stopDAQ(false);
    app.sysTerm();
    return 0;
}
