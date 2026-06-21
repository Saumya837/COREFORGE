// WHAT THIS FILE DOES AND WHY
// ----------------------------
// Day 6 proved a SINGLE atomic operation is indivisible. Today asks a
// different question: does atomicity also guarantee ORDER between
// operations on DIFFERENT atomic variables, as seen by other threads?
// With memory_order_relaxed, the answer is no. The CPU and compiler are
// both allowed to reorder instructions (for performance) as long as it
// doesn't change what THAT thread sees of its OWN operations -- but
// another thread watching from outside can see the effects in a
// different order than you wrote them.
//
// This file runs the classic "store buffer reordering" experiment many
// times in a loop, counting how often we observe the "impossible -
// looking" result (r1==0 && r2==0) with relaxed ordering, then switches
// to memory_order_seq_cst and shows the count drops to zero. The goal
// is to SEE this happen on your own machine, not just read about it.

#include <iostream>
#include <thread>
#include <atomic>

std::atomic<int> x{0};
std::atomic<int> y{0};
int r1, r2;

// Run with whichever memory order is currently active (TODO 1/2)
void thread1() {
    x.store(1, std::memory_order_relaxed);
    r1 = y.load(std::memory_order_relaxed);
}

void thread2() {
    y.store(1, std::memory_order_relaxed);
    r2 = x.load(std::memory_order_relaxed);
}

int main() {
    int impossible_count = 0;
    const int trials = 1000;

    for (int i = 0; i < trials; i++) {
        x.store(0, std::memory_order_relaxed);
        y.store(0, std::memory_order_relaxed);
        r1 = 0;
        r2 = 0;

        //launch thread1 and thread2, join both
        std::thread t1 = std::thread(thread1);
        std::thread t2 = std::thread(thread2);

        t1.join();
        t2.join();

        // if r1 == 0 AND r2 == 0, increment impossible_count
        if(r1 == 0 and r2 == 0)  impossible_count++;
    }
    std::cout << "Impossible-looking result occurred " << impossible_count
              << " out of " << trials << " trials" << std::endl;

    return 0;
}