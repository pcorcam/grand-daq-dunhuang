#include <data_manager.h>
#include <utils.h>
#include <cassert>

using namespace grand;

int main()
{
    DataManager dm;
    bool bOut = false;
    dm.setEventOutput([&bOut](char *data, size_t sz)->void{
        LOG(INFO) << "output event, size = " << sz;
        bOut = true;
    });

    size_t size = 1024;
    char testData[size];

    dm.addEvent(testData, size);
    
    assert(bOut = true);

    return 0;
}
