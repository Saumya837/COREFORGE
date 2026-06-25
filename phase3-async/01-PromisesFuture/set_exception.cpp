// EXERCISE 7 -- set_exception
// -----------------------------
// Exercise 6 was the ACCIDENTAL failure path: promise destroyed, main
// got a generic "broken promise" error -- it knew THAT the worker
// failed, but not WHY.
//
// set_exception is the CONTROLLED version: when a worker knows exactly
// what went wrong (bad input, file missing, etc.), it catches its own
// error and calls prom.set_exception(...) to send THAT SPECIFIC
// exception across to the waiter. main's .get() then re-throws that
// exact exception, which main catches normally -- as if the error had
// happened locally. This is how futures carry failures with full
// fidelity, not just values.
//
// PREDICTION (write before running): when main calls fut.get() on a
// promise that had set_exception() called on it, what happens at the
// .get() line -- does it return a value, return an error code, or
// throw?

#include <iostream>
#include <future>
#include <thread>
#include <stdexcept>

// A worker that does some computation but might fail for a SPECIFIC,
// known reason.
void worker(std::promise<int> prom, int input) {
    try {
        if (input < 0) {
            // a specific, meaningful failure -- not a generic crash
            throw std::invalid_argument("input must be non-negative, got a negative value");
        }
        int result = input * input;

        // success, set the value normally
        prom.set_value(result);
    } catch (...) {
        // The worker caught its own exception. Forward it to
        prom.set_exception(std::current_exception());
    }
}

int main() {
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();

    // pass a NEGATIVE input on purpose, to force the worker's failure path
    std::thread t(worker, std::move(prom), -5);

    // call fut.get() inside a try/catch. Catch
    try {
        int value = fut.get();
    } catch (const std::invalid_argument& e) {
        std::cout<<e.what()<<std::endl;
    }

    t.join();
    return 0;
}
