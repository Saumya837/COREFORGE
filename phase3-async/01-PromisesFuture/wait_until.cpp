// EXERCISE 8 -- wait_for / wait_until (timed waiting)
// ----------------------------------------------------
// Every .get() so far blocks UNCONDITIONALLY -- if the worker never
// finishes, main waits forever. Real systems need "wait up to N
// milliseconds, then give up / retry / warn / do something else."
//
// future::wait_for(duration) waits up to a timeout and returns a
// STATUS, without consuming the value:
//   future_status::ready    -> value is available, .get() is instant
//   future_status::timeout  -> timed out, not ready yet
//   future_status::deferred -> (async-deferred only) hasn't started
//
// wait_for does NOT consume the result like .get() does -- you can
// call it repeatedly, then call .get() once when it's finally ready.
//
// PREDICTION (write before running): the worker sleeps 500ms. If you
// poll with wait_for(100ms) in a loop, roughly how many "timeout"
// statuses do you expect before the first "ready"?

#include <iostream>
#include <future>
#include <thread>
#include <chrono>

int slow_task() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return 99;
}

int main() {
    std::future<int> fut = std::async(std::launch::async, slow_task);

    int timeout_count = 0;

    // loop. Each iteration, call fut.wait_for(100ms) and store
    std::future_status status;
    while (true) {
        status = fut.wait_for(std::chrono::milliseconds(100));
        if(status == std::future_status::ready){
            break;
        }
        std::cout<<"still waiting:"<<std::endl;
        timeout_count++;
    }
    int result = fut.get();
    std::cout << "result = " << result << ", timeouts seen = " << timeout_count << std::endl;


    return 0;
}
