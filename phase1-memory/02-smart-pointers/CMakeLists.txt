project(smart-pointers)

add_executable(test_unique tests/test_unique.cpp)
target_include_directories(test_unique PRIVATE include)

add_executable(test_shared tests/test_shared.cpp)
target_include_directories(test_shared PRIVATE include)

add_executable(test_weak tests/test_weak.cpp)
target_include_directories(test_weak PRIVATE include)