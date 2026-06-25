// future/promise Implementation (the keystone)
// -----------------------------------------------------------
// All week you've used future/promise and trusted that internally they
// "just do the lesson-2 handshake for you." Today you PROVE it by
// building it yourself, using only mutex + condition_variable + a value
// slot -- the exact primitives from lessons 1-2.
//
// The shape is IDENTICAL to lesson 2 (waiter blocks on a flag until a
// notifier sets it). The only new thing: MyPromise and MyFuture are
// SEPARATE objects that must share ONE underlying state. A
// std::shared_ptr<SharedState> is how two objects co-own one
// heap-allocated thing -- both hold a shared_ptr to the same state, so
// when the promise sets the value, the future sees it.
//
// PREDICTION: once built, this should behave exactly like the real
// std::promise/std::future from exercise on day 11a -- get() blocks
// until set_value, then returns the value.

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <chrono>

// The shared state both MyPromise and MyFuture point at.
struct SharedState {
    int value = 0;
    bool ready = false;
    std::mutex mtx;
    std::condition_variable cv;
};

class MyFuture {
public:
    explicit MyFuture(std::shared_ptr<SharedState> state) : state_(state) {}

    int get() {
        // lock the shared state's mutex with a unique_lock,
        std::unique_lock<std::mutex> uniquelock(state_->mtx);
        state_->cv.wait(uniquelock, [this] () {return state_->ready; });
        return state_->value;
    }

private:
    std::shared_ptr<SharedState> state_;
};

class MyPromise {
public:
    MyPromise() : state_(std::make_shared<SharedState>()) {}

    // Hand out a future that shares THIS promise's state.
    MyFuture get_future() {
        return MyFuture(state_);
    }

    void set_value(int v) {
        // lock the mutex
        {
            std::lock_guard<std::mutex> lock(state_->mtx);
            state_->value = v;
            state_->ready = true;
        }
        state_->cv.notify_one();
    }

private:
    std::shared_ptr<SharedState> state_;
};

int main() {
    MyPromise prom;
    MyFuture fut = prom.get_future();

    // worker thread: sleeps, then sets the value (like a slow task)
    std::thread t([&prom]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        prom.set_value(123);
    });

    std::cout << "main: waiting on my own future..." << std::endl;
    int result = fut.get();   // should BLOCK ~500ms, then return 123
    std::cout << "main: got " << result << " from my hand-built future" << std::endl;

    t.join();
    return 0;
}