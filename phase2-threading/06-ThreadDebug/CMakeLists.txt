project(ThreadDebug)

add_executable(Scenario1-LazyInit Scenario1-LazyInit.cpp)
add_executable(Scenario2-unprotectedRead Scenario2-unprotectedRead.cpp)
add_executable(Scenario3-use-after-free Scenario3-use-after-free.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")

