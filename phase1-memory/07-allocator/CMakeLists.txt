# phase1-memory/07-allocator/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(MyDeque)

set(CMAKE_CXX_STANDARD 20)

add_executable(test_allocator tests/test_allocator.cpp)