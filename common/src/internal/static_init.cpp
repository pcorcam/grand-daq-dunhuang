#include <utils.h>
#include <set>

INITIALIZE_EASYLOGGINGPP

namespace dcache {

class StaticInit {
public:
    StaticInit() {
        std::set<std::string> names;
        names.insert("default");
        names.insert("network");
        names.insert("data");
        names.insert("elec");

        char *cfg = getenv("GRAND_DAQ_CONFIG_LOG");

        for(auto n: names) {
            el::Configurations c(std::string(cfg) + "/" + n + ".cfg");
            el::Loggers::getLogger(n);
            el::Loggers::reconfigureLogger(n, c);
        }
    }
};

StaticInit gStaticInit;

}
