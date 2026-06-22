cmake_minimum_required(VERSION 3.10)
project(day9_spinlock)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(spinlock spinlock.cpp)

target_compile_options(spinlock PRIVATE -fsanitize=thread -g)
target_link_options(spinlock PRIVATE -fsanitize=thread)
target_link_libraries(spinlock PRIVATE Threads::Threads)