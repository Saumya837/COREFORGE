// SCENARIO 1: Lazy initialization race
// This pattern is everywhere in production code: a cache, a connection
// pool, a singleton -- "if not ready, set it up" -- done without atomicity.
// Find the bug using ONLY the TSan output. Don't read the code line by
// line looking for it -- read the TSan report first, find the line
// numbers it names, THEN look at those specific lines.

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

std::mutex mtx;
bool initialized = false;
int* shared_resource = nullptr;

void worker(int id) {
    // "check" -- is the resource ready?
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (!initialized) {
            // "act" -- nobody else has set it up, so I will
            if (shared_resource == nullptr) 
                shared_resource = new int(42);
            initialized = true;
        }

    }
   

    // everyone assumes shared_resource is valid by this point
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "worker " << id << " sees value: " << *shared_resource << std::endl;
}

int main() {
    std::vector<std::thread> workers;
    for (int i = 0; i < 8; i++) {
        workers.push_back(std::thread(worker, i));
    }
    for (auto& w : workers) {
        w.join();
    }
    delete shared_resource;
    std::cout << "main: done" << std::endl;
    return 0;
}
