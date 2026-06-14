cmake_minimum_required(VERSION 3.10)
project(MyStack)

set(CMAKE_CXX_STANDARD 20)

add_executable(test_stack tests/test_stack.cpp)

target_include_directories(test_stack PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/phase1-memory/03-MyVector/include
)

target_compile_features(test_stack PRIVATE cxx_std_17)