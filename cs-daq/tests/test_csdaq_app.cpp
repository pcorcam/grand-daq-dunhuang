#include <csdaq_app.h>

using namespace grand;

int main()
{
    CSDAQApp app;
    app.sysInit();
    app.startDAQ();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    //No du client, the stop method will be block by sending messages to DUs
    //app.stopDAQ();
    //app.sysTerm();
    return 0;
}
