cmake_minimum_required(VERSION 3.16)
project(rage_fxc_extractor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(rage_fxc_extractor rage_fxc_extractor.cpp)

# Install target
install(TARGETS rage_fxc_extractor RUNTIME DESTINATION bin)
