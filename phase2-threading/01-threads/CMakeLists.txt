project(threads)

add_executable(first_thread 01-Intro/First_thread.cpp)

add_executable(waitflag 02-waitflag/WaitFlag.cpp)

add_executable(spsc 03-ProducerConsumer/spsc.cpp)
add_executable(mpmc 03-ProducerConsumer/mpmc.cpp)


target_compile_options(first_thread PRIVATE -fsanitize=thread -g)
target_link_options(first_thread PRIVATE -fsanitize=thread)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")

