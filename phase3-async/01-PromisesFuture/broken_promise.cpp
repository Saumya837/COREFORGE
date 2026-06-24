// EXERCISE 6 -- broken promise
// ------------------------------
// This answers the question YOU flagged in exercise 4: "what if the
// error occurs and the value never gets set?"
//
// A future waiting on .get() trusts that someone will eventually call
// set_value() on the matching promise. But if the promise is destroyed
// WITHOUT ever setting a value (worker crashed, threw, or just went out
// of scope), the future can't wait forever for a value that will never
// come. The standard library handles this: when a promise is destroyed
// with no value set, its destructor stores a "broken promise" error
// into the shared state, and the next future::get() THROWS
// std::future_error instead of hanging forever.
//
// PREDICTION (write before running): will main's fut.get() hang
// forever, or will something stop it?

#include <iostream>
#include <future>
#include <thread>

int main() {
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();

    // launch a worker thread that takes the promise BY MOVE,
    // but DELIBERATELY never calls set_value() on it. Simulate a
    // worker that "started, then failed/returned early before
    // producing a result." The simplest version: a lambda that takes
    // the promise by move and just... does nothing with it, then
    // returns (letting the promise be destroyed when the lambda ends).
    //
    std::thread t([p = std::move(prom)]() mutable {
    });


    // TODO 2: call fut.get() inside a try/catch. Catch
    try {
       int result =  fut.get();
    } catch (const std::future_error& e) {
        std::cout<<e.what();
    }

    // join the thread
    t.join();

    return 0;
}

// EXPECTED: instead of hanging forever, fut.get() throws, your catch
// block fires, and you see a message containing "broken promise".
// That's the library refusing to let you wait on a value that can
// never arrive.