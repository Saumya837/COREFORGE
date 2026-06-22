cmake_minimum_required(VERSION 3.10)
project(day10b_detach_demo)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
find_package(Threads REQUIRED)
 
add_executable(detach_demo detach_demo.cpp)
 
target_compile_options(detach_demo PRIVATE -fsanitize=thread -g)
target_link_options(detach_demo PRIVATE -fsanitize=thread)
target_link_libraries(detach_demo PRIVATE Threads::Threads)