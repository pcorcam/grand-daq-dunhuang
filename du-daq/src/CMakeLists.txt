
#add_library(dudaq SHARED 
#    IFrontend.cpp
#    ScopeA.cpp
#    ScopeDummy.cpp
#)

add_library(dudaq SHARED
        data_manager.cpp
	du_fsm.cpp
	data_manager.cpp
        dudaq_app.cpp
        frontend.cpp
        scope_a.cpp
        scope_dummy.cpp
        data_format.cpp
	template_flt_online/utils.cpp
        template_flt_online/error_handling.cpp
        template_flt_online/template_FLT.cpp
        tflite_inference.cpp
)

install(TARGETS dudaq LIBRARY DESTINATION lib)
