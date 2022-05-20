#pragma once

#include <functional>
#include <unistd.h>

namespace grand {

typedef std::function<void(char *data, size_t sz)> EventOutput;

class DataManager {
public:
    DataManager();
    ~DataManager();
    void setEventOutput(EventOutput fun);

    /**
     * @brief called by readout thread
     *
     * @param data electronic event data
     */
    void addEvent(char *data, size_t sz);

    /**
     * @brief called by backend command input thread
     *
     * @param accept message from cs-daq
     */
    void accept(char *data, size_t sz);

    void initialize();
    void terminate();

private:

    EventOutput m_eventOutputFun;

    // TEMP:
    char *m_eventBuffer;

    /**
     * @brief generate T2 package from the electronic event data
     *
     * @param data electronic event data
     */
    void generateT2(char *data, size_t sz);
};

}

