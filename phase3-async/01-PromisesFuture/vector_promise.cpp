// EXERCISE 4 -- does future/promise need manual memory ordering?
// ----------------------------------------------------------------
// Day 8 proved that a flag + ordinary data needs explicit
// release/acquire to GUARANTEE the data is visible once the flag is
// seen. promise/future are built internally on a mutex + condition_
// variable -- the same kind of handshake. This file tests that
// guarantee directly, with a much bigger and more "checkable" payload
// than a single int: a 100,000-element vector with a known pattern.
//
// PREDICTION (write this down BEFORE running, in your own notes or
// just out loud to yourself): across 20 runs, do you expect this
// vector to ever come back even slightly wrong?

#include <iostream>
#include <future>
#include <thread>
#include <vector>

int main() {
    std::promise<std::vector<int>> prom; 
    //promise is just a construct which takes long,
    // kind of event whgich can be used to do something it could be fileread, or waiting for sever to respond

    std::future<std::vector<int>> fut = prom.get_future(); // just like a place holder nothing else, what input the the promise returns

    std::thread t([&prom]() {
        std::vector<int> data(100000);
        for (int i = 0; i < 100000; i++) {
            data[i] = i;   // known, checkable pattern
        }
        prom.set_value(data); //once this released
        //but the question if the error occurs in that event and fails to get future then 
    });

    std::vector<int> result = fut.get(); //it will acquire happens before concept like memory orderind released-acquired

    bool all_correct = true;
    for (int i = 0; i < 100000; i++) {
        if (result[i] != i) {
            all_correct = false;
            break;
        }
    }

    std::cout << (all_correct ? "CORRECT" : "CORRUPTED") << std::endl;

    t.join();
    return 0;
}

// RUN THIS 20 TIMES. A shell loop is much faster than running it
// manually 20 times:
//   for i in $(seq 1 20); do ./vector_through_future; done
//
// Report: how many times CORRECT, how many times CORRUPTED, out of 20.