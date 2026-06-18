cmake_minimum_required(VERSION 3.10)
project(SpinLock)

set(CMAKE_CXX_STANDARD 20)

add_executable(SpinLock 
    tests/test_spinLock.cpp 
    src/SpinLock.cpp
)

target_include_directories(SpinLock PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SpinLock pthread)