# phase1-memory/05-MyDeque/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(MyDeque)

set(CMAKE_CXX_STANDARD 20)

add_executable(test_deque tests/test_deque.cpp)
target_include_directories(test_deque PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

