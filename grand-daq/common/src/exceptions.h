#pragma once

#include <exception>
#include <string>
#include <sstream>

using std::string;

namespace grand {

class GrandException: public std::exception
{
public:
    GrandException(int code, const string baseMsg, const string msg) : m_baseMsg(baseMsg), m_errMsg(msg) , m_errCode(code) {}
    virtual string what() {
        std::stringstream ss;
        if(baseMsg() != "") {
            ss << baseMsg() << ": ";
        }
        ss << msg() << "(" << "code: " << std::hex << code() << std::dec << ")";
        return ss.str();
    }

    int code() { return m_errCode; }
    string baseMsg() { return m_baseMsg; }
    string msg() { return m_errMsg; }

protected:
    string m_baseMsg;
    string m_errMsg;
    int m_errCode;
};

}

#define DECLARE_EXCEPTION(ClassName, ErrorCode, BaseMsg) \
    namespace grand { \
        class ClassName : public grand::GrandException { \
            public: \
            ClassName(string msg) : grand::GrandException(ErrorCode, BaseMsg, msg) {} \
        }; \
    }

DECLARE_EXCEPTION(NetworkError, 1001, "network error");

