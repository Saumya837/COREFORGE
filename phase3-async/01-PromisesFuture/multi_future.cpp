// WHAT THIS FILE DOES AND WHY
// ----------------------------
// Day 11a: you built promise/future by hand -- created the promise,
// extracted the future, manually moved the promise into a thread.
// std::async does that plumbing for you in one call.
//
// Internally, EVERY future/promise uses a mutex + condition_variable to
// guard the result. set_value() does the equivalent of a release;
// get() does the equivalent of an acquire. This is WHY you never write
// manual memory_order code when using futures -- the library already
// did that handshake for you, every time. Today proves this directly:
// 3 concurrent tasks, zero atomics, zero manual ordering, and the
// result is still guaranteed correct.

#include <iostream>
#include <future>
#include <vector>
#include <chrono>
#include <thread>

int slow_square(int x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return x * x;
}

int main() {
    int result;
    std::vector<std::future<int>> futures;
    std::vector<int> inputs = {3, 5, 7};

    auto start = std::chrono::steady_clock::now();

    // TODO 1: for each input, launch std::async(std::launch::async,
    // slow_square, input) and push the returned future into 'futures'
    for (int x : inputs) {
        futures.emplace_back(std::async(std::launch::async, slow_square, x));
    }

    std::cout << "main: all 3 tasks launched, futures collected" << std::endl;

    // TODO 2: loop over 'futures' IN ORDER, call .get() on each, print
    // the result as it arrives
    for (auto& fut : futures) {
        result = fut.get();
        std::cout<<result<<std::endl;
    }

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Total time: " << elapsed << "ms" << std::endl;

    // ANSWER BEFORE RUNNING: each slow_square sleeps 500ms. If these 3
    // tasks ran truly sequentially, total time would be ~1500ms. If
    // they ran in parallel, total time would be ~500ms (plus a little
    // overhead). Predict which one you'll see, THEN run it and check.

    return 0;
}