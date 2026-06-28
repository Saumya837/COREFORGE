// ============================================================================
// EXERCISE 6 — ITERATOR INVALIDATION (concurrent modify)   [light hints]
// ============================================================================
//
// SYMPTOM
//   Crashes quickly. One thread walks a shared std::vector with iterators while
//   another push_back()s into it. A push_back that grows capacity REALLOCATES
//   the buffer and frees the old one — the walking iterators now dangle.
//
// BUILD & RUN
//   make ex6_iterator_invalidation
//   ./ex6_iterator_invalidation
//
// GDB HINTS
//   - run; on the crash `bt` lands you in the reader's loop.
//   - `print v.size()` and compare to where the iterator points — the iterator
//     is walking memory that was freed and reused when the writer reallocated.
//   - `thread apply all bt` shows the writer mid-push_back at the same time.
//   - ThreadSanitizer flags this as a data race on the vector, too.
//
// CONCEPT
//   std::vector iterators (and pointers/refs into it) are invalidated by any
//   operation that can reallocate, e.g. push_back past capacity. A container
//   that is read and written concurrently needs external synchronization, or a
//   concurrent data structure designed for it.
//
// YOUR TASK — FIX IT
//   Guard every access to `v` with a shared std::mutex (lock around the whole
//   read loop and around each write). Or redesign so the reader works on a
//   snapshot/copy taken under the lock.
// ============================================================================

#include <iostream>
#include <thread>
#include <vector>

std::vector<int> v(1000, 1);   // shared, UNSYNCHRONIZED
std::mutex mtx;

void reader() {
    long sum = 0;

    std::lock_guard<std::mutex> lread(mtx);
    for (int round = 0; round < 10000; ++round) {
        // BUG: these iterators are invalidated the instant the writer
        // reallocates the buffer underneath us.
        for (auto it = v.begin(); it != v.end(); ++it)
            sum += *it;        // dereferences a possibly-freed iterator
    }
    std::cout << "sum = " << sum << '\n';
}

void writer() {
    std::lock_guard<std::mutex> lwrite(mtx);
    for (int i = 0; i < 200000; ++i)
        v.push_back(i);        // growth reallocates → frees reader's buffer
}

int main() {
    std::thread r(reader);
    std::thread w(writer);
    r.join();
    w.join();
    return 0;
}