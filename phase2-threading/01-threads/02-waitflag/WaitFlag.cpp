#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

// Shared state between the two threads
bool ready = false;

// declare a std::mutex here (same idea as Day 1)
std::mutex mtx;

std::mutex print_mtx;

// declare a std::condition_variable here
std::condition_variable cv;


// Thread A: the WAITER
// This thread should block until 'ready' becomes true, then print "A: proceeding"
void waiter() {
    // create a std::unique_lock<std::mutex> on your mutex
    std::unique_lock<std::mutex> uniLock(mtx);
  
    // call cv.wait(lock, predicate)
    cv.wait(uniLock, []{return ready;});

    {
        std::lock_guard<std::mutex> lock(print_mtx);
        std::cout << "A: proceeding" << std::endl;
    }
}

// Thread B: the NOTIFIER
// This thread sleeps briefly, then sets ready=true and wakes up the waiter
void notifier() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    //lock the mutex (lock_guard is fine here — no waiting happens
    // in this function, so the simpler RAII lock works)
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }

    // call cv.notify_one()

    cv.notify_one();
    
    {
        std::lock_guard<std::mutex> lock(print_mtx);
        std::cout << "B: set ready and notified" << std::endl;
    }
}

int main() {
    // TODO 9: create two threads: one running waiter, one running notifier
    std::thread w1 = std::thread(waiter);
    std::thread n1 = std::thread(notifier);

    // TODO 10: join both threads
    w1.join();
    n1.join();

    std::cout << "main: done" << std::endl;
    return 0;
}