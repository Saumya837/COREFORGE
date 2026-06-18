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

// ============================================================
// SECTION 2: AtomicFlag
//
// std::atomic_flag is the ONLY C++ atomic guaranteed to be
// lock-free on EVERY platform, even embedded systems.
//
// It has exactly two operations:
//   test_and_set()  — atomically set to true, return OLD value
//   clear()         — atomically set to false
//
// x86: test_and_set → LOCK BTS (Bit Test and Set) or XCHG
//
// This is the primitive your SpinLock (Topic 3) is built on.
// ============================================================

class AtomicFlag {
public:
    AtomicFlag() {
        // ATOMIC_FLAG_INIT ensures the flag starts CLEAR (false).
        // In C++20 you can also default-construct to cleared state.
        flag_.clear();
    }

    // Returns TRUE if the flag WAS already set (someone else owns it).
    // Returns FALSE if we just acquired it (it was clear before we set it).
    // x86: LOCK XCHG [flag], 1  (exchange byte with 1, return old)
    bool test_and_set() {
        return flag_.test_and_set(std::memory_order_seq_cst);
    }

    // Release: set flag back to false.
    // x86: MOV [flag], 0  (with a store-fence for seq_cst)
    void clear() {
        flag_.clear(std::memory_order_seq_cst);
    }

    // C++20: test without setting
    bool test() const {
        return flag_.test(std::memory_order_seq_cst);
    }

private:
    std::atomic_flag flag_;
};

// ============================================================
// SECTION 3: LockFreeCounter
//
// A counter safe for concurrent increment from N threads.
// No locks. No sleep. Just hardware atomics.
//
// minidb use case: buffer manager pin counter.
// When a backend pins a buffer (needs to read/write it), it
// atomically increments pin_count. Clock sweep skips buffers
// with pin_count > 0. No mutex needed — the atomic IS the lock.
//
// PostgreSQL equivalent:
//   pg_atomic_fetch_add_u32(&bufHdr->state, BUF_REFCOUNT_ONE)
// ============================================================

class LockFreeCounter {
public:
    explicit LockFreeCounter(int init = 0) : count_(init) {}

    // Increment and return new value.
    // We use fetch_add which returns the OLD value, so +1 for new.
    int increment() {
        return count_.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    // Decrement. Returns new value.
    int decrement() {
        return count_.fetch_add(-1, std::memory_order_relaxed) - 1;
    }

    // Snapshot — relaxed is fine for statistics; we don't need
    // to synchronise other memory operations with this read.
    int get() const {
        return count_.load(std::memory_order_relaxed);
    }

    // Reset to zero. seq_cst so all prior ops are visible after.
    void reset() {
        count_.store(0, std::memory_order_seq_cst);
    }

    // ---- CAS-based bounded increment ----
    // Only increment if current value < max.
    // Returns true if incremented, false if already at max.
    // This is how PostgreSQL limits max connections per buffer group.
    bool increment_if_less_than(int max_val) {
        int current = count_.load(std::memory_order_relaxed);
        while (current < max_val) {
            // Try to swap current → current+1
            // If CAS fails (another thread changed it), current is updated
            // to the NEW value automatically — loop and retry.
            if (count_.compare_exchange_weak(
                    current,          // expected (updated on failure)
                    current + 1,      // desired
                    std::memory_order_seq_cst,
                    std::memory_order_relaxed)) {
                return true;          // we incremented it
            }
            // current now holds the fresh value — retry the comparison
        }
        return false;  // already at max_val
    }

private:
    std::atomic<int> count_;
};

// ============================================================
// SECTION 4: LockFreeBufferPinCount
//
// Miniature model of PostgreSQL's buffer pin management.
// BufferDesc.state packs multiple fields into one atomic uint32:
//
//  bits 0-17 : pin count (max 262143 concurrent pinners)
//  bit  18   : dirty flag
//  bit  19   : valid flag
//  bit  20   : io_in_progress
//
// All updated atomically with fetch_or/fetch_and/CAS.
// This is why PostgreSQL can handle thousands of concurrent
// buffer accesses without a global mutex.
// ============================================================

struct MiniBufferState {
    static constexpr uint32_t PIN_COUNT_MASK = 0x0003FFFFu;  // bits 0-17
    static constexpr uint32_t BIT_DIRTY      = (1u << 18);
    static constexpr uint32_t BIT_VALID      = (1u << 19);
    static constexpr uint32_t PIN_ONE        = 1u;            // add 1 to pin count

    std::atomic<uint32_t> state{0};

    // Pin the buffer (increment pin count)
    void pin() {
        state.fetch_add(PIN_ONE, std::memory_order_acquire);
    }

    // Unpin the buffer
    void unpin() {
        // fetch_sub with release: our reads of the buffer data are done.
        // Another thread observing pin_count==0 (acquire) will see our
        // reads completed. This is acquire/release pairing in practice.
        state.fetch_sub(PIN_ONE, std::memory_order_release);
    }

    // Mark buffer dirty (we modified it)
    void mark_dirty() {
        state.fetch_or(BIT_DIRTY, std::memory_order_relaxed);
    }

    uint32_t pin_count() const {
        return state.load(std::memory_order_relaxed) & PIN_COUNT_MASK;
    }

    bool is_dirty() const {
        return (state.load(std::memory_order_relaxed) & BIT_DIRTY) != 0;
    }

    bool is_valid() const {
        return (state.load(std::memory_order_relaxed) & BIT_VALID) != 0;
    }
};
