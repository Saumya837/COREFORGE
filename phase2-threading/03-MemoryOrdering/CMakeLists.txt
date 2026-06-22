project(MemoryOrdering)

add_executable(relaxedSeq relaxedSeq.cpp)
add_executable(acquireRelease acquireRelease.cpp)

target_compile_options(first_thread PRIVATE -fsanitize=thread -g)
target_link_options(first_thread PRIVATE -fsanitize=thread)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")