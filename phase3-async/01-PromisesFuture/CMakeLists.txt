cmake_minimum_required(VERSION 3.10)
project(day11a_promise_future)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
find_package(Threads REQUIRED)
 
add_executable(promise_future promise_future.cpp)
 
target_compile_options(promise_future PRIVATE -fsanitize=thread -g)
target_link_options(promise_future PRIVATE -fsanitize=thread)
target_link_libraries(promise_future PRIVATE Threads::Threads)