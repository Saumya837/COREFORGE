// SCENARIO 2: "Just a quick read" race
// A shared counter is correctly protected everywhere it's WRITTEN.
// But one place reads it "just to log progress" without the lock,
// because "reading is harmless." Find it from the TSan report.

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

std::mutex mtx;
std::mutex print_mtx;
long total_bytes_processed = 0;

void worker(int id) {
    for (int i = 0; i < 50000; i++) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            total_bytes_processed += 1024;
        }

        // "just logging progress every so often, reading is harmless"
        if (i % 10000 == 0) {
            std::lock_guard<std::mutex> lock(mtx);
            std::lock_guard<std::mutex> plock(print_mtx);
            std::cout << "worker " << id << " progress, total so far: "
                      << total_bytes_processed << std::endl;
        }
    }
}

int main() {
    std::vector<std::thread> workers;
    for (int i = 0; i < 4; i++) {
        workers.push_back(std::thread(worker, i));
    }
    for (auto& w : workers) {
        w.join();
    }
    std::cout << "main: final total = " << total_bytes_processed << std::endl;
    return 0;
}