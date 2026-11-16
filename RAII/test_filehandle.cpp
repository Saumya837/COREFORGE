// test_file_handle.cpp — tests for the FileHandle exercise (Lesson 1: RAII)
//
// Build (Linux only — uses /proc/self/fd and /dev/full):
//   g++ -std=c++17 -Wall -Wextra -g -fsanitize=address,undefined test_file_handle.cpp -o test_fh
//   ./test_fh
//
// Bonus (after Lesson 2, move semantics):
//   g++ -std=c++17 -Wall -Wextra -g -fsanitize=address,undefined -DTEST_MOVE test_file_handle.cpp -o test_fh
//
// Expects your class in "file_handle.hpp" in the same folder.

#include "file_handle.hpp"

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <stdexcept>
#include <string>
#include <system_error>
#include <type_traits>
#include <unistd.h>

// ---------- tiny test framework ----------
static int g_failed = 0, g_passed = 0;
#define CHECK(cond)                                                              \
    do {                                                                         \
        if (cond) { ++g_passed; }                                                \
        else { ++g_failed; std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } \
    } while (0)
#define TEST(name) static void name()
#define RUN(name) do { std::printf("[ RUN  ] %s\n", #name); name(); } while (0)

// Counts entries in /proc/self/fd (includes the fd opendir itself uses, which is
// fine: we only ever compare two counts taken the same way).
static int open_fd_count() {
    DIR* d = opendir("/proc/self/fd");
    if (!d) { std::perror("opendir /proc/self/fd"); return -1; }
    int n = 0;
    while (readdir(d)) ++n;
    closedir(d);
    return n;
}

static const char* TMP = "/tmp/fh_test.bin";

// ---------- compile-time checks ----------
static_assert(!std::is_copy_constructible_v<FileHandle>, "FileHandle must not be copyable");
static_assert(!std::is_copy_assignable_v<FileHandle>,    "FileHandle must not be copy-assignable");

// ---------- tests ----------

// T1: opening a valid file gives an open handle and exactly one new fd.
TEST(t1_open_valid) {
    int before = open_fd_count();
    FileHandle f(TMP, "wb");
    CHECK(f.is_open());
    CHECK(f.get() != nullptr);
    CHECK(open_fd_count() == before + 1);
}

// T2: leaving scope closes the file.
TEST(t2_scope_exit_closes) {
    int before = open_fd_count();
    {
        FileHandle f(TMP, "wb");
        CHECK(open_fd_count() == before + 1);
    }
    CHECK(open_fd_count() == before);
}

// T3: an exception thrown inside the scope still closes the file
//     (the fd is gone by the time we are inside catch).
TEST(t3_exception_closes) {
    int before = open_fd_count();
    int inside_catch = -1;
    try {
        FileHandle f(TMP, "wb");
        throw std::runtime_error("boom");
    } catch (const std::runtime_error&) {
        inside_catch = open_fd_count();
    }
    CHECK(inside_catch == before);
}

// T4: opening a missing file throws std::system_error with ENOENT, and leaks nothing.
TEST(t4_open_failure_throws) {
    int before = open_fd_count();
    bool threw = false;
    try {
        FileHandle f("/tmp/definitely/not/here.bin", "rb");
    } catch (const std::system_error& e) {
        threw = true;
        CHECK(e.code().value() == ENOENT);
    }
    CHECK(threw);
    CHECK(open_fd_count() == before);
}

// T5: explicit close() returns 0, is idempotent, and the destructor does not close twice.
TEST(t5_explicit_close) {
    int before = open_fd_count();
    {
        FileHandle f(TMP, "wb");
        CHECK(f.close() == 0);
        CHECK(!f.is_open());
        CHECK(f.get() == nullptr);
        CHECK(open_fd_count() == before);
        CHECK(f.close() == 0);          // second close: no-op, still 0
    }                                    // destructor must NOT fclose again (ASan catches it)
    CHECK(open_fd_count() == before);
}

// T6: data written through get() is really in the file after close().
TEST(t6_write_then_read_back) {
    {
        FileHandle w(TMP, "wb");
        for (uint64_t i = 0; i < 4; ++i)
            CHECK(std::fwrite(&i, sizeof i, 1, w.get()) == 1);
        CHECK(w.close() == 0);
    }
    FileHandle r(TMP, "rb");
    uint64_t v = 99, expected = 0;
    bool all_ok = true;
    while (std::fread(&v, sizeof v, 1, r.get()) == 1) {
        if (v != expected) all_ok = false;
        ++expected;
    }
    CHECK(all_ok);
    CHECK(expected == 4);
}

// T7: close() reports the error that only appears at flush time.
//     /dev/full accepts the buffered fwrite, then fails with ENOSPC on flush.
TEST(t7_close_reports_flush_error) {
    FileHandle f("/dev/full", "wb");
    uint64_t x = 42;
    CHECK(std::fwrite(&x, sizeof x, 1, f.get()) == 1);   // only went into the libc buffer
    errno = 0;
    int rc = f.close();
    CHECK(rc == EOF);
    CHECK(errno == ENOSPC);
    CHECK(!f.is_open());                                  // closed even though it failed
}

// T8: the gen.cpp bug — an early return in the middle of writing leaks nothing.
static bool write_records_with_early_exit(const char* path, uint64_t n, uint64_t fail_at) {
    FileHandle f(path, "wb");
    for (uint64_t i = 0; i < n; ++i) {
        if (i == fail_at) return false;                   // simulated fwrite failure
        if (std::fwrite(&i, sizeof i, 1, f.get()) != 1) return false;
    }
    return f.close() == 0;
}
TEST(t8_early_return_no_leak) {
    int before = open_fd_count();
    for (int k = 0; k < 2000; ++k)                        // would hit EMFILE with a leak at ulimit 1024
        CHECK(!write_records_with_early_exit(TMP, 10, 5));
    CHECK(open_fd_count() == before);
    CHECK(write_records_with_early_exit(TMP, 10, 100));   // success path
    CHECK(open_fd_count() == before);
}

#ifdef TEST_MOVE
static_assert(std::is_nothrow_move_constructible_v<FileHandle>, "move ctor should be noexcept");
static_assert(std::is_nothrow_move_assignable_v<FileHandle>,    "move assignment should be noexcept");

// B1: move construction transfers ownership; the source becomes empty.
TEST(b1_move_construct) {
    int before = open_fd_count();
    FileHandle a(TMP, "wb");
    FILE* raw = a.get();
    FileHandle b(std::move(a));
    CHECK(!a.is_open());
    CHECK(a.get() == nullptr);
    CHECK(b.get() == raw);
    CHECK(open_fd_count() == before + 1);                 // still exactly one file
}

// B2: move assignment closes the target's old file first.
TEST(b2_move_assign_closes_old) {
    int before = open_fd_count();
    {
        FileHandle a(TMP, "wb");
        FileHandle b("/tmp/fh_test_2.bin", "wb");
        CHECK(open_fd_count() == before + 2);
        b = std::move(a);
        CHECK(open_fd_count() == before + 1);             // b's old file was closed
        CHECK(!a.is_open());
        CHECK(b.is_open());
    }
    CHECK(open_fd_count() == before);
}

// B3: a function can return a FileHandle (ownership leaves the function).
static FileHandle open_for_write(const char* p) { return FileHandle(p, "wb"); }
TEST(b3_return_from_function) {
    int before = open_fd_count();
    {
        FileHandle f = open_for_write(TMP);
        CHECK(f.is_open());
        CHECK(open_fd_count() == before + 1);
    }
    CHECK(open_fd_count() == before);
}
#endif

int main() {
    RUN(t1_open_valid);
    RUN(t2_scope_exit_closes);
    RUN(t3_exception_closes);
    RUN(t4_open_failure_throws);
    RUN(t5_explicit_close);
    RUN(t6_write_then_read_back);
    RUN(t7_close_reports_flush_error);
    RUN(t8_early_return_no_leak);
#ifdef TEST_MOVE
    RUN(b1_move_construct);
    RUN(b2_move_assign_closes_old);
    RUN(b3_return_from_function);
#endif
    std::remove(TMP);
    std::remove("/tmp/fh_test_2.bin");
    std::printf("\n%d passed, %d failed\n", g_passed, g_failed);
    return g_failed == 0 ? 0 : 1;
}