cmake_minimum_required(VERSION 3.10)
project(NovaCore)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)

file(GLOB SRC_FILES
    src/*.cpp
)

add_executable(NovaCore ${SRC_FILES})
