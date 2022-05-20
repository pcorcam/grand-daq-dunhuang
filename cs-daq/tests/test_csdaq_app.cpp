#include <csdaq_app.h>

using namespace grand;

int main()
{
    CSDAQApp app;
    app.sysInit();
    app.startDAQ();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    app.stopDAQ();
    app.sysTerm();
}
