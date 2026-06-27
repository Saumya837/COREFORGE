project(ThreadDebug)

add_executable(Scenario1-LazyInit Scenario1-LazyInit.cpp)
add_executable(Scenario2-unprotectedRead Scenario2-unprotectedRead.cpp)
add_executable(Scenario3a-deadlock Scenario3a-deadlock.cpp)
add_executable(Scenario4-datarace Scenario4-datarace.cpp)
add_executable(scenario5-deadlock scenario5-deadlock.cpp)
add_executable(scenario6-lostwakeup scenario6-lostWakeup.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")

