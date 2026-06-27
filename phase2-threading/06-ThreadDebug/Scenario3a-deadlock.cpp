// Read this code. Do NOT run it yet.
//
// Predict: does this deadlock? If so, trace through exactly what
// sequence of events causes it -- which thread holds what, waiting for
// what, when. Write your trace down before building/running.

#include <iostream>
#include <thread>
#include <mutex>

std::mutex mutex_a;
std::mutex mutex_b;

void thread1_work() {
    std::lock_guard<std::mutex> lock_a(mutex_a);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::lock_guard<std::mutex> lock_b(mutex_b);
    std::cout << "thread1: got both locks" << std::endl;
}

void thread2_work() {
    std::lock_guard<std::mutex> lock_b(mutex_a);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::lock_guard<std::mutex> lock_a(mutex_b);
    std::cout << "thread2: got both locks" << std::endl;
}

int main() {
    std::thread t1(thread1_work);
    std::thread t2(thread2_work);
    t1.join();
    t2.join();
    std::cout << "main: done" << std::endl;
    return 0;
}