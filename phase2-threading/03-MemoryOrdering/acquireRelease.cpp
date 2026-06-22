// WHAT THIS FILE DOES AND WHY
// ----------------------------
// Day 7 showed relaxed gives no cross-thread ordering guarantee. Today
// is the fix: memory_order_release (on a store) paired with
// memory_order_acquire (on the matching load) gives a GUARANTEED
// happens-before relationship -- the standard promises it, not "it
// usually works on this CPU." This is the exact mechanism std::mutex
// uses internally, and it's the pattern Day 9/10's spinlock and
// futex-mutex are built on.
//
// The pattern: Thread A writes ordinary data, THEN does a release-store
// on a flag. Thread B spins on an acquire-load of that flag, and the
// moment it sees true, it is GUARANTEED to see every ordinary write A
// made before the release. No flag, no guarantee -- this is the whole
// point of today.
//
// This file deliberately runs the pattern TWO ways: once with
// release/acquire (correct), once with relaxed/relaxed (the Day 7
// problem, reintroduced on purpose) so you can compare directly.

#include <iostream>
#include <thread>
#include <atomic>

int data = 0;                  // ordinary, NON-atomic data
std::atomic<bool> flag{false}; // the synchronization flag

void writer() {
    data = 42;  // ordinary write -- this is the data we care about

    // store 'true' into flag using memory_order_release
    flag.store(true, std::memory_order_release);
}

void reader() {
    // TODO 2: spin-wait: keep loading 'flag' with memory_order_acquire
    while(!flag.load(std::memory_order_acquire));
    //print 'data' here
    std::cout << "data = " << data << std::endl;
}

int main() {
    std::thread t1(writer);
    std::thread t2(reader);
    t1.join();
    t2.join();
    return 0;
}