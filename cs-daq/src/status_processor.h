#pragma once

#include <string>

namespace grand {

class StatusProcessor {
public:
    StatusProcessor();

    void processData(std::string du, char *data, size_t sz);
};

}
