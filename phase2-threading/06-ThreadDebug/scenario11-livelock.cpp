// ============================================================================
// EXERCISE 10 — LIVELOCK (capstone)                        [minimal hints]
// ============================================================================
//
// SYMPTOM
//   The program never finishes, but it is NOT deadlocked — a CPU core is pinned
//   at 100% and the threads are very busy. They just never make progress. This
//   is livelock: each thread grabs its first lock, fails to get the second,
//   politely releases and retries... in perfect lockstep with the other, so
//   both keep retrying forever.
//
// BUILD & RUN
//   make ex10_livelock
//   ./ex10_livelock          # spins forever, no output. Ctrl-C to kill.
//   (Notice CPU usage: a deadlock idles; a livelock burns CPU.)
//
// GDB HINTS (you're on your own for the fix)
//   - run; Ctrl-C several times in a row and `bt` each time: the threads are
//     not parked in a futex (like a deadlock) — they're cycling through the
//     try-lock / back-off / retry path.
//   - `print attempts1` / `print attempts2`: huge and climbing. Lots of work,
//     zero progress — the signature of livelock.
//
// CONCEPT
//   Deadlock = everyone stuck waiting. Livelock = everyone busy reacting to
//   each other and getting nowhere. Naive "release and retry" back-off without
//   any randomness lets two threads stay synchronized indefinitely.
//
// YOUR TASK — FIX IT (research, then pick one)
//   - Impose a global lock ORDER and just block (no try/back-off) — or use
//     std::scoped_lock(a, b), which acquires both deadlock-free.
//   - Or keep the back-off but add randomized/exponential wait before retrying
//     so the two threads fall out of lockstep.
// ============================================================================

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
using namespace std::chrono_literals;

std::mutex m1, m2;
std::atomic<bool> finished{false};
std::atomic<long> attempts1{0}, attempts2{0};

void worker(std::mutex& first, std::mutex& second, std::atomic<long>& attempts) {
    while (!finished) {
        std::unique_lock<std::mutex> l1(first);          // take our first lock
        ++attempts;
        std::this_thread::sleep_for(1ms);                // ...let the other grab its first
        std::unique_lock<std::mutex> l2(second, std::try_to_lock);
        if (!l2.owns_lock()) {
            l1.unlock();                                 // BUG: polite back-off, but
            continue;                                    //      in lockstep → forever
        }
        finished = true;                                 // got both — would do work
        std::cout << "acquired both locks, did the work\n";
    }
}

int main() {
    // Opposite first-lock per thread, like the deadlock — but here they try_lock
    // and retry, which turns the deadlock into a livelock instead.
    std::thread t1(worker, std::ref(m1), std::ref(m2), std::ref(attempts1));
    std::thread t2(worker, std::ref(m2), std::ref(m1), std::ref(attempts2));
    t1.join();
    t2.join();
    std::cout << "attempts: " << attempts1 << " / " << attempts2 << '\n';
    return 0;
}