#pragma once

namespace grand {

class DataManager {
public:
    DataManager();
    void initialize();
    void terminate();

private:
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

    /**
     * @brief generate T2 package from the electronic event data
     *
     * @param data electronic event data
     */
    void generateT2(char *data, size_t sz);
};

}

