// ============================================================================
// GDB WALKTHROUGH 
// ----------------------------------------------------------------------------
//
//   gdb ./ex1_data_race        # start GDB with the program loaded
//
//   (gdb) break worker         # set a breakpoint on the worker function
//   (gdb) run                  # start the program; it stops at the breakpoint
//
//   The first thread to call worker() hits the breakpoint. Now look around:
//
//   (gdb) info threads         # list ALL threads; '*' marks the current one
//                              # the Frame column shows where each one is
//   (gdb) thread 3             # switch the current thread to #3
//   (gdb) bt                   # backtrace: the call stack of *this* thread
//   (gdb) info locals          # local variables in the current frame
//   (gdb) print counter        # inspect the shared global
//
//   Run a command across every thread at once:
//
//   (gdb) thread apply all bt  # backtrace of every thread (your #1 tool)
//
//   Watch the shared variable change (a "watchpoint" stops on writes):
//
//   (gdb) watch counter        # NOTE: in this loop it fires constantly, so it
//   (gdb) continue             # is SLOW here. Try it once to feel it, then:
//   (gdb) delete               # remove all breakpoints/watchpoints
//
//   A conditional breakpoint stops only when an expression is true. The store
//   line is hit constantly, so condition it on the loop index to land in the
//   middle of the work (replace LINE with the line number of `counter = tmp+1`,
//   which `list worker` will show you):
//
//   (gdb) break ex1_data_race.cpp:LINE if i == 1000
//   (gdb) continue             # stops the first time some thread's i hits 1000
//   (gdb) info threads         # now inspect the other threads mid-flight
//
//   Other essentials you'll reuse everywhere:
//   (gdb) next  / n            # run one source line (step OVER calls)
//   (gdb) step  / s            # run one source line (step INTO calls)
//   (gdb) finish               # run until the current function returns
//   (gdb) continue / c         # resume until the next stop
//   (gdb) frame 1              # select stack frame #1 from the backtrace
//   (gdb) quit                 # leave GDB
//
//   TIP: `set scheduler-locking on` freezes the other threads so only the
//   current one runs while you step — very handy for understanding one thread
//   without the others racing ahead. Turn it `off` again before you continue.
//
// YOUR TASK
//   1. Use the GDB commands above until you're comfortable moving between
//      threads and reading their stacks.
//   2. Then FIX the race: make `counter` a std::atomic<long>, OR guard the
//      increment with a std::mutex + std::lock_guard. Rebuild and confirm the
//      result is now always 1,000,000.
// ============================================================================

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

std::atomic<int> counter = 0; 
std::mutex mut;

void worker(int iterations) {
    for (int i = 0; i < iterations; ++i) {
        std::lock_guard<std::mutex> lock(mut);
        long tmp = counter;            // load  (read-modify-write, step 1)
        std::this_thread::yield();     // <-- only widens the window so the race
                                       //     is reliably visible on any machine,
                                       //     even a single core. The bug is the
                                       //     missing synchronization, not this.
        counter = tmp + 1;             // store (step 3) — overwrites with a
    }                                  // possibly-stale value → lost updates
}

int main() {
    const int num_threads = 4;
    const int iters = 250'000;   // expected total = 1,000,000

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i)
        threads.emplace_back(worker, iters);

    for (auto& t : threads)
        t.join();

    const long expected = static_cast<long>(num_threads) * iters;
    std::cout << "counter  = " << counter  << '\n';
    std::cout << "expected = " << expected << '\n';
    if (counter != expected)
        std::cout << "BUG: lost " << (expected - counter) << " increments\n";
    else
        std::cout << "OK\n";
    return 0;
}