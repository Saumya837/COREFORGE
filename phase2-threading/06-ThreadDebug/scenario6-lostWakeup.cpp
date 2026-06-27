// ============================================================================
// EXERCISE 3 — LOST WAKEUP (condition variable)   [lighter hints — your turn]
// ============================================================================
//
// SYMPTOM
//   The program prints nothing and hangs. The consumer is waiting forever even
//   though the producer already did its job.
//
// BUILD & RUN
//   make ex3_lost_wakeup
//   ./ex3_lost_wakeup         # hangs. Ctrl-C to kill.
// ============================================================================

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex m;
std::condition_variable cv;
bool ready = false;

void consumer() {
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [] {return ready;});                    
    std::cout << "consumer proceeding, ready=" << ready << '\n';
}

void producer() {
    {
        std::lock_guard<std::mutex> lk(m);
        ready = true;
    }
    cv.notify_one();                // happens before consumer waits → lost
}

int main() {
    std::thread p(producer);
    // Give the producer a head start so its notify fires before the consumer
    // reaches cv.wait(). This makes the lost wakeup reproducible every run.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::thread c(consumer);

    p.join();
    c.join();   // hangs until you fix the wait
    std::cout << "done\n";
    return 0;
}