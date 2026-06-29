// ============================================================================
// EXERCISE 10 — LIVELOCK (capstone)                        [minimal hints]
// ============================================================================
//
// SYMPTOM
//   The program never finishes, but it is NOT deadlocked — the threads are very
//   busy (CPU stays hot) yet make zero progress. Each grabs its first lock,
//   fails to get the second, politely releases and retries... in lockstep with
//   the other, forever.
//
//   IMPORTANT — why this version is engineered:
//   A *naive* symmetric back-off livelock (just "lock first; try second; on fail
//   release and retry") is METASTABLE — it can resolve on its own. The threads
//   start staggered and the OS scheduler jitters them out of lockstep within a
//   few rounds; the instant they desync, one thread's try_to_lock on the second
//   lock succeeds and it finishes. So the plain version often completes in a
//   handful of attempts and does NOT spin forever. (That is itself a real and
//   useful lesson about livelocks: they are frequently transient.)
//
//   To make the livelock reliable enough to *study*, this file removes the two
//   sources of drift:
//     1) a 2-thread barrier (sync2) re-aligns both threads at the start of every
//        round, so they can't fall out of phase across rounds; and
//     2) a holding[] handshake makes each thread wait until BOTH hold their
//        first lock before either tries the second — guaranteeing the second
//        try always fails.
//   With those, every round fails for both threads: a persistent livelock.
//
// BUILD & RUN
//   make ex10_livelock
//   ./ex10_livelock          # spins forever, no output. Ctrl-C to kill.
//   (Watch CPU: a deadlock idles in a futex; a livelock burns a core.)
//
// GDB HINTS (you're on your own for the fix)
//   - run; Ctrl-C a few times and `bt`: the threads are NOT parked in a futex
//     wait — they're cycling through the barrier / try-lock / back-off path.
//   - `print attempts1` / `print attempts2`: large and climbing. Maximum effort,
//     zero progress — the signature of livelock.
//
// CONCEPT
//   Deadlock = everyone stuck waiting. Livelock = everyone busy reacting to each
//   other and getting nowhere. Symmetric "release and retry" back-off, kept in
//   lockstep, never resolves.
//
// YOUR TASK — FIX IT (research, then pick one)
//   - Impose a global lock ORDER and just block (no try/back-off) — or use
//     std::scoped_lock(a, b), which acquires both deadlock-free. (The barrier and
//     handshake go away in the fixed version; they exist only to make the BUG
//     reproducible.)
//   - Breaking the symmetry is the key: asymmetric ordering, or randomized /
//     exponential back-off so the two threads stop moving in lockstep.
// ============================================================================

#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex m1;
std::mutex m2;
std::atomic<long> attempts1{0}, attempts2{0};


void worker(std::mutex& first, std::mutex& second, std::atomic<long>& attempts) 
{
    std::scoped_lock lock(first, second);      // both grab their first lock
    ++attempts;
    std::cout << "acquired both locks, did the work\n";   // never reached
    return;
}

int main() {
    std::thread t1(worker, std::ref(m1), std::ref(m2), std::ref(attempts1));
    std::thread t2(worker, std::ref(m2), std::ref(m1), std::ref(attempts2));
    t1.join();
    t2.join();
    return 0;
}