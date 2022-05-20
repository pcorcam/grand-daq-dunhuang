
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
link_libraries(grandcommon)

add_subdirectory(src)
add_subdirectory(tests)

