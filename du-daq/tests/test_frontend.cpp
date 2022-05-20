//**********************************//
//******Created by duanbh,
//******2022/4/22
//**********************************//
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cassert>
#include <scope_dummy.h>

using namespace grand;

int main()
{
    ScopeDummy scopeA;
    scopeA.initialize();
    scopeA.configure();
    scopeA.start();
    sleep(1);
    scopeA.stop();
    return 0;
}
