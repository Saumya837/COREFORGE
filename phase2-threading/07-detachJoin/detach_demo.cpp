// WHAT THIS FILE DOES AND WHY
// ----------------------------
// This is Lesson 1's exact counter race, solved a completely different
// way: no std::mutex, no lock_guard, no locking AT ALL. std::atomic<int>
// guarantees that operations like ++ are indivisible at the HARDWARE
// level -- the CPU itself won't let another core observe or interfere
// with the operation halfway through. There's nothing to "lock" because
// there's no multi-step region to protect -- the whole operation IS one
// step, as far as any other thread can ever observe.
//
// This matters for storage work specifically: lock-free queues, MVCC
// version counters, buffer-pool reference counts are all built on
// atomics, not mutexes, because locking on every single increment is
// too slow under heavy contention. Today is the first step into that
// world.

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

// declare counter as std::atomic<int> instead of plain int
// (compare this single line to Lesson 1's "int counter = 0;")
std::atomic<int> counter = 0;


void increment() {
    for (int i = 0; i < 100000; i++) {
        // increment the atomic counter
        counter++;
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.push_back(std::thread(increment));
    }
    for (auto& t : threads) {
        t.detach();
    }

    // print the final counter value
    std::cout<<counter; 

    return 0;
}