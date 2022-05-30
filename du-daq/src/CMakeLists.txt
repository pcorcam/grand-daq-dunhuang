
#add_library(dudaq SHARED 
#    IFrontend.cpp
#    ScopeA.cpp
#    ScopeDummy.cpp
#)

add_library(dudaq SHARED
        du_fsm.cpp
        data_manager.cpp
        dudaq_app.cpp
        frontend.cpp
        scope_a.cpp
        scope_dummy.cpp
)

install(TARGETS dudaq LIBRARY DESTINATION .)
