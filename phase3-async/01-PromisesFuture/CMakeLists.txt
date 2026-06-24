cmake_minimum_required(VERSION 3.10)
project(day11a_promise_future)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
find_package(Threads REQUIRED)
 
add_executable(promise_future promise_future.cpp)
add_executable(multi_future multi_future.cpp)
add_executable(vector_promise vector_promise.cpp)
add_executable(shared_promise shared_promise.cpp)
add_executable(broken_promise broken_promise.cpp)
 
target_compile_options(promise_future PRIVATE -fsanitize=thread -g)
target_link_options(promise_future PRIVATE -fsanitize=thread)
target_link_libraries(promise_future PRIVATE Threads::Threads)