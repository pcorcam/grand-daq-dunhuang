/**
Created by duanbh,
2022/4/25
**/
#pragma once

#include <std_types.h>
#include <frontend.h>

namespace grand {

class ScopeDummy : public IFrontend {
public:
    ScopeDummy();
    ~ScopeDummy();
protected:
    virtual void elecInit() {}
    virtual void elecConfig() {}
    virtual void elecStartRun() {}
    virtual void elecStopRun() {}
    virtual void elecTerminate() {}

    virtual int readData(char *data, size_t maxSize) {
        for(int i=0; i<128; i++) {
            data[i] = (unsigned char)(unsigned int)i;
        }
        return 128;
    }

    virtual void terminateElectronics() {}
};

}
