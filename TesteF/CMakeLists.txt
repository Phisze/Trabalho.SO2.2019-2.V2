cmake_minimum_required(VERSION 3.14)
project(TesteF C)

set(CMAKE_C_STANDARD 99)

find_package(Threads REQUIRED)

add_executable(TesteF main.c)
target_link_libraries(TesteF Threads::Threads)
