
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/template_flt_online)

link_libraries(grandcommon)

add_subdirectory(src)
add_subdirectory(exec)
add_subdirectory(tests)

