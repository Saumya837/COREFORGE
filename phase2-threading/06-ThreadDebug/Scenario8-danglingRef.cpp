// ============================================================================
// EXERCISE 5 — DANGLING REFERENCE / USE-AFTER-FREE        [light hints]
// ============================================================================
//
// SYMPTOM
//
// BUILD & RUN
//       make 
//         ex5_dangling_ref 
//                 ./ex5_dangling_ref
//
// GDB HINTS
//
// THE RIGHT TOOL HERE
//
// CONCEPT
//
// YOUR TASK — FIX IT
//      
// ============================================================================

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
using namespace std::chrono_literals;

        
void start(std::thread& out) 
{
    std::vector<int> data(1000, 7);     
    out = std::thread([data = std::move(data)]() mutable
    {
        std::this_thread::sleep_for(100ms);
        {
            for (int i = 0; i < 1000; i++)
                data.emplace_back(i);  
            long sum = 0;
            for (int x : data) sum += x;
            std::cout << "sum = " << sum << '\n';  
        }
    }); 
};   
//data vector gets destrioyed here                                   

int main() {
    std::thread t;
    start(t);
    t.join();
    return 0;
}