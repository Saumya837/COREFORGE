// EXERCISE 5 -- std::shared_future
// ----------------------------------
// future is single-consumer: only ONE thread may ever call .get() on
// it (Day 11a proved this -- the second call threw future_error).
// shared_future removes that restriction: get a shared_future by
// calling .share() on a future, and it can be COPIED (future cannot be
// copied, only moved) and .get() can be called on it many times, from
// many threads, all reading the same eventual value.
//
// PREDICTION (write this down before running): will all 4 reader
// threads successfully print 42, or will some throw future_error the
// way Day 11a's second .get() did?

#include <iostream>
#include <future>
#include <thread>
#include <vector>
#include <chrono>

int main() {
    std::promise<int> prom;
    //get a std::shared_future<int> from the promise.
    std::shared_future<int> shared_fut = prom.get_future().share();

    std::vector<std::thread> readers;

    // TODO 2: launch 4 reader threads. Each one should:
    //   - capture shared_fut BY VALUE (not by reference!) -- each
    //     thread gets its own cheap copy, which is what makes calling
    //     .get() from multiple threads safe. Capture i by value too.
    //   - call shared_fut.get() to receive the value (this BLOCKS until
    //     the promise is set)
    //   - print "reader <i> got <value>"
    for (int i = 0; i < 4; i++) {
        readers.emplace_back(std::thread([shared_fut, i] (){
            int val = shared_fut.get();
            std::cout<< "reader " << i << " got " << val << "\n";
        }));
    }

    // small delay so the readers are genuinely waiting before we set
    // the value -- makes the "all 4 were really blocked" timing visible
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // TODO 3: set the promise's value to 42 -- this unblocks ALL 4
    // readers at once
    prom.set_value(42);


    // TODO 4: join all reader threads
    for (int i = 0; i < 4; i++) {
        readers[i].join();
    }


    return 0;
}

// RUN IT a few times. All 4 lines should print "got 42", no exceptions,
// no crash, regardless of which order the 4 threads print in.