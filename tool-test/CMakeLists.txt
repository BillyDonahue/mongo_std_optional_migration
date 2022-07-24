cmake_minimum_required(VERSION 3.13.4)
project(TestInput)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(input SHARED input.cpp)
target_include_directories(input PUBLIC "../../mongo/src/third_party/boost")
target_link_libraries(input "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")
