project(ThreadDebug)

add_executable(Scenario1-LazyInit Scenario1-LazyInit.cpp)
add_executable(Scenario2-unprotectedRead Scenario2-unprotectedRead.cpp)
add_executable(Scenario3a-deadlock Scenario3a-deadlock.cpp)
add_executable(Scenario4-datarace Scenario4-datarace.cpp)
add_executable(deadlock Scenario5-deadlock.cpp)
add_executable(lostwakeup Scenario6-lostWakeup.cpp)
add_executable(raceCondition Scenario7-raceCondition.cpp)
add_executable(danglingRef Scenario8-danglingRef.cpp)
add_executable(ItrInvalid Scenario9-ItrInvalid.cpp)
add_executable(SelfDeadlock Scenario10-SelfDeadlock.cpp)
add_executable(livelock Scenario11-livelock.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")

