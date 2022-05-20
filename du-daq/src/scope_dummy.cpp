#include <scope_dummy.h>

using namespace grand;

ScopeDummy::ScopeDummy(){
    std::cout << "This is Son class" << std::endl;
}

ScopeDummy::~ScopeDummy(){
    std::cout << "Son Obj is being deleted" << std::endl;
}
