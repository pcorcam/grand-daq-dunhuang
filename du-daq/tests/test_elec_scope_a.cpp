//**********************************//
//******Created by duanbh,
//******2022/4/22
//**********************************//
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cassert>
#include <scope_a.h>
#include <fstream>

using namespace grand;

#define SHADOW_SIZE 2000

int main()
{
    char sl[SHADOW_SIZE];
    std::ifstream inF;
    inF.open("/tmp/shadow_list.dat", std::ifstream::binary);
    inF.read(sl, SHADOW_SIZE);
    inF.close();

    ScopeA scopeA;
    scopeA.initialize();
    scopeA.configure(sl);
    scopeA.start();
    sleep(1);
    scopeA.stop();
    return 0;
}
