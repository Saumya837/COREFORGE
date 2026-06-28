// ============================================================================
// EXERCISE 4 — RACE CHALLENGE (intermittent crash)        [minimal hints]
// ============================================================================
//
// SYMPTOM
//   It crashes with a segfault, usually within a run or two — sometimes on the
//   first. Intermittent-looking timing bugs like this are the real-world kind;
//   this one is your exam.
//
// BUILD & RUN
//   make ex4_race_challenge
//   ./ex4_race_challenge      # crashes; rerun if a run happens to survive
//
// GDB FOR INTERMITTENT CRASHES (figure out the rest yourself)
//   - Just `run` under gdb and wait for the crash; gdb stops AT the fault.
//   - `bt` shows where it died; `info threads` + `thread apply all bt` show
//     what the others were doing. Inspect the container's state at the fault.
//   - If a single run won't crash, loop it from the shell:
//         while ./ex4_race_challenge; do :; done
//     and run that same loop body under gdb, or just rerun `run` in gdb.
//
// THE QUESTION TO ANSWER
//   pop() checks q.empty() and only THEN takes the lock. Two threads can both
//   pass the empty() check when exactly one element remains. What happens when
//   the second one calls front()/pop() on an empty queue? Where must the lock
//   actually start for the check and the action to be one atomic step?
//
// NO FIX SPELLED OUT THIS TIME. Make the check-and-pop a single critical
// section so empty() and front()/pop() can't be split by another thread.
// ============================================================================

#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

std::queue<int> q;
std::mutex m;

// BUG: the empty() check is OUTSIDE the lock (check-then-act / TOCTOU race).
int pop() {
    {
        std::lock_guard<std::mutex> lk(m);     // Fixed 
        if (!q.empty()) {                          
            std::this_thread::yield();             
                if (q.empty()) {
                    volatile int* boom = nullptr;
                    return *boom;                      // SIGSEGV — "the race bit us"
                }
                int v = q.front();            
                q.pop();
                return v;
            }
    }
    return -1;
}

int main() {
    for (int round = 0; round < 100000; ++round) {
        // one item, several threads racing to grab it
        {
            std::lock_guard<std::mutex> lk(m);
            std::queue<int> empty;
            std::swap(q, empty);
            q.push(42);
        }

        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i)
            threads.emplace_back([]{ pop(); });
        for (auto& t : threads)
            t.join();
    }
    std::cout << "survived all rounds\n";
    return 0;
}

//Understand the code behaviour