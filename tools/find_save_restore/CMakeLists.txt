project("find_save_restore")
set(CMAKE_CXX_STANDARD 17)

add_executable(find_save_restore "find_save_restore.cpp")

target_link_libraries(find_save_restore PRIVATE XenonUtils)
