// ============================================================================
// EXERCISE 7 — SELF-DEADLOCK (re-entrant lock)             [light hints]
// ============================================================================
//
// SYMPTOM
//   Hangs. But unlike Exercise 2 there is no lock-ORDER cycle between threads —
//   a single thread deadlocks against ITSELF by locking the same non-recursive
//   std::mutex twice. One "thread-safe" method takes the lock and then calls a
//   second "thread-safe" method that takes the same lock again.
//
// BUILD & RUN
//   make ex7_self_deadlock
//   ./ex7_self_deadlock       # prints a little, then hangs. Ctrl-C to kill.
//
// GDB HINTS
//   - run; Ctrl-C; `thread apply all bt`.
//   - You'll see ONE worker thread stuck in a lock acquisition, and walking its
//     stack shows the SAME thread already inside a frame that holds that lock.
//     A thread blocked on a mutex it already owns = re-entrant self-deadlock.
//
// CONCEPT
//   std::mutex is NOT recursive: locking it twice from one thread is undefined
//   behavior and in practice deadlocks. Public methods that each lock, then
//   call each other, are the usual culprit.
//
// YOUR TASK — FIX IT
//   Preferred: split the logic so the lock is taken ONCE — have the locked
//   public method call a private helper that assumes the lock is already held
//   (and does NOT lock). Avoid std::recursive_mutex unless you truly need it;
//   reaching for it is usually a sign the design should be refactored.
// ============================================================================

#include <iostream>
#include <mutex>
#include <thread>
#include <atomic>

std::atomic<bool> flag{false};

class Counter {
    private:
        std::mutex m;
        int value = 0;

        // BUG: this re-locks the SAME mutex while increment() still holds it.
        void reset_if_even() {
                value = value >= 10 ? 0: value;
            }     // lock #2 on same thread → hang

    public:
        void increment() {
            std::lock_guard<std::mutex> lk(m);     // lock #1
            ++value;
            std::cout << "value = " << value << '\n';
            if (value % 2 == 0)
                reset_if_even();               
        // calls a method that locks again
        }
};


int main() {
    std::cout << std::unitbuf;   // flush each line so progress shows before hang
    Counter c;
    std::thread worker([&c] {
        for (int i = 0; i < 5; ++i)
            c.increment();                     // deadlocks when value first even
    });
    worker.join();
    std::cout << "done\n";
    return 0;
}