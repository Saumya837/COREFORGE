
// What we build:
//   1. AtomicInt     — pedagogical wrapper exposing every operation
//   2. AtomicFlag    — guaranteed lock-free flag (maps to test_and_set)
//   3. LockFreeCounter — first real lock-free data structure
//   4. Validation    — 4 threads × 250K increments = exactly 1,000,000
// ============================================================
 
#include <atomic>
#include <pthread.h>
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstring>
 
// ============================================================
// SECTION 1: AtomicInt
//
// Wraps std::atomic<int> to make every operation explicit and
// name the underlying hardware instruction it maps to.
//
// Memory orders we use here (more detail in Topic 2):
//   memory_order_seq_cst  — full fence, sequential consistency (safest)
//   memory_order_relaxed  — no fence, just atomicity (fastest)
//   memory_order_acquire  — see all writes before a paired release
//   memory_order_release  — publish all our writes to acquire observers
// ============================================================
 
class AtomicInt {
public:
    // Constructor: value is zero-initialised
    explicit AtomicInt(int init = 0) : value_(init) {}
 
    // --- LOAD ---
    // x86:  MOV EAX, [addr]  (+ compiler fence for seq_cst)
    // ARM:  LDR R0, [addr]   (+ DMB memory barrier for seq_cst)
    int load() const {
        return value_.load(std::memory_order_seq_cst);
    }
 
    // Relaxed load — no fence, just atomicity. Use when ordering
    // does NOT matter (e.g. reading a statistics counter).
    int load_relaxed() const {
        return value_.load(std::memory_order_relaxed);
    }
 
    // --- STORE ---
    // x86:  XCHG [addr], reg  OR  MOV + MFENCE for seq_cst
    // ARM:  STL  R0, [addr]   (store-release)
    void store(int desired) {
        value_.store(desired, std::memory_order_seq_cst);
    }
 
    // --- FETCH_ADD ---
    // Returns OLD value, adds delta atomically.
    // x86:  LOCK XADD [addr], reg
    //       (XADD = eXchange-and-ADD: swaps reg↔mem, then adds)
    // ARM:  LDREX/ADD/STREX retry loop
    int fetch_add(int delta) {
        return value_.fetch_add(delta, std::memory_order_seq_cst);
    }
 
    // Relaxed fetch_add: when you only care about the final total,
    // not the order relative to other memory operations.
    // This is what PostgreSQL uses for per-backend statistics counters.
    int fetch_add_relaxed(int delta) {
        return value_.fetch_add(delta, std::memory_order_relaxed);
    }
 
    // --- COMPARE-AND-EXCHANGE (CAS) ---
    // THE fundamental primitive. Everything lock-free is built on CAS.
    //
    // Semantics:
    //   if (*this == expected) { *this = desired; return true; }
    //   else                   { expected = *this; return false; }
    //
    // x86:  LOCK CMPXCHG [addr], desired
    //       (compares EAX with [addr]; if equal, stores desired)
    // ARM:  LDREX/CMP/STREXEQ loop
    //
    // "strong" variant: guaranteed to succeed if *this == expected.
    // "weak"   variant: may SPURIOUSLY FAIL even if equal (for LL/SC).
    //                   Use weak in retry loops for better ARM perf.
    bool compare_exchange_strong(int& expected, int desired) {
        return value_.compare_exchange_strong(
            expected, desired,
            std::memory_order_seq_cst,   // success order
            std::memory_order_seq_cst    // failure order
        );
    }
 
    // CAS weak — use this in retry loops
    bool compare_exchange_weak(int& expected, int desired) {
        return value_.compare_exchange_weak(
            expected, desired,
            std::memory_order_seq_cst,
            std::memory_order_relaxed    // failure just re-reads, no fence needed
        );
    }
 
    // --- FETCH_OR / FETCH_AND ---
    // PostgreSQL uses these to pack flags into buffer header state.
    // BufferDesc.state is a uint32 with bits for pin count, dirty, valid:
    //   pg_atomic_fetch_or_u32(&buf->state, BM_DIRTY)  — set dirty bit
    //   pg_atomic_fetch_and_u32(&buf->state, ~BM_PIN)  — clear pin bit
    // x86: LOCK OR [addr], reg  /  LOCK AND [addr], reg
    int fetch_or(int mask) {
        return value_.fetch_or(mask, std::memory_order_seq_cst);
    }
    int fetch_and(int mask) {
        return value_.fetch_and(mask, std::memory_order_seq_cst);
    }
 
    // Raw access for inspection (don't use in production!)
    std::atomic<int>& raw() { return value_; }
 
private:
    // std::atomic<int> MUST be aligned to its size for LOCK prefix to work.
    // On x86-64 this is 4 bytes. The compiler handles this automatically.
    // If you ever allocate atomics in a custom memory pool, ensure alignment!
    alignas(std::atomic<int>) std::atomic<int> value_;
};