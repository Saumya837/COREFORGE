project(atomic)
add_executable(atomic-counter 05-Intro/atomic-counter.cpp)
add_executable(relaxed 06-MemoryOrdering/Relaxed.cpp)

target_compile_options(first_thread PRIVATE -fsanitize=thread -g)
target_link_options(first_thread PRIVATE -fsanitize=thread)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")