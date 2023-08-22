#include <utils.h>

using namespace grand;

int main()
{
    std::string s = stringFormat("%d %s", "hello", 123);
    assert(s == "hello123");
    return 0;
}
