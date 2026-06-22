// WHAT THIS FILE DOES AND WHY
// ----------------------------
// All month, threads communicated through shared variables YOU
// protected (mutex, condvar, atomics). std::promise/std::future are a
// standard-library wrapper around exactly that pattern -- one thread
// computes a result and "sets" it on a promise; another thread calls
// .get() on the matching future and BLOCKS until the value is ready.
// Internally this is built from a mutex and condition variable, not
// new magic. You're learning a convenience API on top of lesson 2,
// not a new primitive. This is the missing piece day 12's thread pool
// needs: submit() has to hand a result back to whoever called it.
//
// A promise/future pair is a ONE-SHOT channel: set_value() exactly
// once, get() exactly once. Calling either twice throws.

#include <iostream>
#include <thread>
#include <future>
#include <chrono>

// A "slow computation" -- pretend this takes real work
int slow_square(int x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return x * x;
}

void worker(std::promise<int> result_promise, int input) {
    int result = slow_square(input);

    // set the result on the promise
    result_promise.set_value(result);

}

int main() {
    //  create a std::promise<int>
    std::promise<int> prom;

    // get its associated future BEFORE moving the promise
    std::future<int> fut = prom.get_future();


    //launch the worker thread, MOVING the promise into it
    std::thread t(worker, std::move(prom), 7);


    std::cout << "main: worker launched, doing other work while we wait..." << std::endl;

    //call fut.get() -- this BLOCKS until worker calls set_value()
    try{
        int result = fut.get();
        std::cout << "main: got result = " << result <<std::endl;
    } 
    catch (const std::future_error& e){
        std::cout << "future error : " << e.what() <<std::endl;
    }

    t.join();

    return 0;
}
