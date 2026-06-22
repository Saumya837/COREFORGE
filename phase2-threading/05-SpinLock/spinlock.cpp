// WHAT THIS FILE DOES AND WHY
// ----------------------------
// You've used std::mutex all week without knowing what's inside it.
// Today you build the simplest real lock that exists -- a spinlock --
// from nothing but std::atomic<bool> and compare_exchange_weak. No OS
// calls, no blocking, no std::mutex anywhere.
//
// A spinlock has one atomic flag: locked or unlocked. lock() repeatedly
// tries to atomically flip it from false->true; if that fails (someone
// else holds it), it just tries again immediately in a tight loop --
// burning CPU the whole time, hence "spin". unlock() sets it back to
// false. This is directly built on yesterday's release/acquire pattern:
// whatever the lock-holder wrote inside the critical section must be
// visible to whoever acquires the lock next.
//
// Today's exercise reuses Lesson 1's exact counter race -- 10 threads,
// 100000 increments each, expected total 1,000,000 -- but protected by
// YOUR spinlock instead of std::mutex. Same test, different lock,
// built from atomics instead of taken from the standard library.

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

class SpinLock {
public:
    void lock() {
        bool expected = false;
        // TODO 1: loop calling locked_.compare_exchange_weak(expected, true, ...)
        // until it succeeds.
        //
        // Signature reminder:
        //   locked_.compare_exchange_weak(expected, desired, success_order, failure_order)
        //
        // - expected: false  (we only want to succeed if currently UNLOCKED)
        // - desired:  true   (we're claiming the lock)
        // - success_order: memory_order_acquire  (we're about to enter the
        //     critical section -- need to see everything the previous
        //     holder wrote before their unlock)
        // - failure_order: memory_order_relaxed  (failing tells us nothing
        //     about ordering -- we're just going to retry)

        while (!locked_.compare_exchange_weak(expected, true,
                                                std::memory_order_acquire,
                                                std::memory_order_relaxed)) {
            //reset expected back to false here before the next
            expected = false;
        }
    }

    void unlock() {
        // set locked_ back to false, using memory_order_release
        locked_.store(false, std::memory_order_release);
    }

private:
    std::atomic<bool> locked_{false};
};

SpinLock spin_mtx;
int counter = 0;

void increment() {
    for (int i = 0; i < 100000; i++) {
        spin_mtx.lock();
        // increment counter here (the actual critical section --
        counter++;
        spin_mtx.unlock();
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.push_back(std::thread(increment));
    }
    for (auto& t : threads) {
        t.join();
    }
    std::cout << "Final counter: " << counter << std::endl;
    return 0;
}