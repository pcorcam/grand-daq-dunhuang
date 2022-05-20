
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

link_libraries(yaml-cpp grandcommon)

add_subdirectory(src)
add_subdirectory(exec)
add_subdirectory(tests)

