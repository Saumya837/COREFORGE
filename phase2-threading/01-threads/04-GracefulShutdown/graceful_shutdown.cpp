// WHAT THIS FILE DOES AND WHY
// ----------------------------
// Lesson 3b's shutdown relied on a hardcoded total (40 items) known in
// advance. Real producers don't announce a count -- they just run until
// they're done, then need to SIGNAL that explicitly. Today: a 'done'
// flag, set once after all producers finish, that consumers check
// alongside the queue's contents.
//
// The one rule that matters today: a consumer may ONLY exit when the
// queue is EMPTY *and* done is true. Checking 'done' alone is wrong --
// items can still be sitting in the queue the instant done flips to
// true, and exiting early means those items never get processed. This
// predicate -- "!queue.empty() || done" -- is what cv.wait() waits on,
// and the ACTUAL exit decision inside the loop is separate and stricter:
// only exit if the queue came up empty AND done is true.

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>

std::queue<int> q;
std::mutex mtx;
std::condition_variable cv;
std::mutex print_mtx;

// declare a shared bool 'done', initialized to false.
bool done = false; 


// threads, each pushes 10 unique values (producer_id*100 + i),
void producer(int producer_id) {
    for (int i = 0; i < 10; i++) {
        {
            // TODO 2: lock mtx, push producer_id * 100 + i onto q
            std::lock_guard<std::mutex> plock(mtx);
            q.push(producer_id * 100 + i);
        }
        // notify_one() after releasing the lock
        cv.notify_one();
    }

    //print right when this producer's work is actually finished
    {
        std::lock_guard<std::mutex> plock(print_mtx);
        std::cout << ">>> producer " << producer_id << " FINISHED" << std::endl;
    }
}

// CONSUMER: loops until the queue is empty AND done is true.
// Must NOT exit just because done is true if items remain.
void consumer(int consumer_id) {
    while (true) {
        int value;
        bool should_break = false;
        {
            // unique_lock on mtx
            std::unique_lock<std::mutex> consLock(mtx);

            // cv.wait(lock, predicate) -- predicate should be
            cv.wait(consLock, [] {return !q.empty() || done; });

            // NOW decide, while still holding the lock:
            if(q.empty() && done){
                should_break = true;
            }
            else{
                value = q.front();
                q.pop();
            }
        }  // lock released here

        // if should_break, exit the loop now, BEFORE printing
        if(should_break)
            break;

        {
            std::lock_guard<std::mutex> plock(print_mtx);
            std::cout << "consumer " << consumer_id
                      << " processed " << value << std::endl;
        }
    }
}

int main() {
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    //launch 4 producer threads (producer_id 0..3)
    for(int i = 0; i<4; i++){
        producers.push_back(std::thread(producer, i));
    }

    // launch 4 consumer threads (consumer_id 0..3)
    for(int i = 0; i<4; i++){
        consumers.push_back(std::thread(consumer, i));
    }


    //  producer threads here, BEFORE setting done.
    for(int i = 0; i<4; i++)
        producers[i].join();

     // NOW set done = true (under the lock!) and notify_all()
    {
        std::lock_guard<std::mutex> lock(mtx);
        done = true;
    }
    
    cv.notify_all();
    
    // join all consumer threads
    for(int i = 0; i<4; i++)
        consumers[i].join();

    std::cout << "main: done, " << q.size() << " items left in queue (should be 0)" << std::endl;
    return 0;
}