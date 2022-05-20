#include <csdaq_app.h>
#include <utils.h>

int main(int argc, char **argv)
{
    LOG(INFO) << "DU DAQ is runing..";

    grand::DUDaq app;
    app.sysInit();
    return 0;
}
