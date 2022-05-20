
#add_library(dudaq SHARED 
#    IFrontend.cpp
#    ScopeA.cpp
#    ScopeDummy.cpp
#)

add_library(dudaq SHARED
        frontend.cpp
        #scope_a.cpp
        scope_dummy.cpp
)
