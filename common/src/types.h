#pragma once 

#include <string>
#include <functional>

namespace grand {

typedef uint32_t MessageType;
typedef std::function<void(std::string duID, char *data, size_t sz)> MessageProcessor;

}
