add_executable(decode_t3_data_Version_app decode_t3_data.cpp)

install(TARGETS decode_t3_data_Version_app RUNTIME DESTINATION bin)

add_executable(decode_raw_data_Version_app decode_raw_data.cpp)

install(TARGETS decode_raw_data_Version_app RUNTIME DESTINATION bin)
