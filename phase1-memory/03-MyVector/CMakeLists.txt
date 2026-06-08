cmake_minimum_required(VERSION 3.20)

add_executable(test_vector tests/test_vector.cpp)

target_include_directories(test_vector PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(test_vector PRIVATE cxx_std_17)

