#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

// Shared between producer and consumer
std::queue<int> q;

// declare a std::mutex to protect 'q'
std::mutex mtx;

// declare a std::condition_variable for the consumer to wait on
std::condition_variable cv;

// Optional: a separate mutex just for printing (you found out why on Day 2 -
// std::cout is itself shared state across threads)
std::mutex print_mtx;

// PRODUCER: pushes integers 0..19 onto the queue, one every 50ms
void producer() {
    for (int i = 0; i < 20; i++) {
        {
            //lock the queue's mutex (lock_guard is fine
            std::lock_guard<std::mutex> lock(mtx);
            q.push(i);
        }

        cv.notify_one();
        {
            std::lock_guard<std::mutex> plock(print_mtx);
            std::cout << "produced: " << i << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// CONSUMER: pops and prints values as they arrive, 20 total
void consumer() {
    for (int i = 0; i < 20; i++) {
        int value;
        {
            {
                //  create a unique_lock on the queue's mutex
                std::unique_lock<std::mutex> uniLock(mtx);

                //cv.wait(lock, predicate) -- predicate should check
                cv.wait(uniLock, []{return !q.empty();});


                // read q.front() into 'value', then q.pop()
                value = q.front();
                q.pop();
            }
        }

        {
            std::lock_guard<std::mutex> plock(print_mtx);
            std::cout << "consumed: " << value << std::endl;
        }
    }
}

int main() {
    // TODO 9: create producer and consumer threads
    std::thread p1 = std::thread(producer);
    std::thread c1 = std::thread(consumer);

    // TODO 10: join both
    p1.join();
    c1.join();

    std::cout << "main: done" << std::endl;
    return 0;
}