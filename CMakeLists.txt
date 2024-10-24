cmake_minimum_required(VERSION 3.17)
project(watcher)

set(CMAKE_CXX_STANDARD 14)

include_directories(G:\\boost_1_86_0)

add_executable(server server.cpp)

target_link_libraries(server ws2_32)

