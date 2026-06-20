// WHAT THIS FILE DOES AND WHY
// ----------------------------
// Lesson 3a had exactly one producer and one consumer. Today there are 4 of
// each, all sharing the same queue, the same mutex, and the same condvar.
// The mutex/condvar PATTERN does not change at all from Day 3 -- what
// changes is that with multiple waiters, notify_one() wakes SOME consumer,
// not "the" consumer, and that consumer might wake up to find the queue
// already emptied by a different consumer that got there first. This is
// why cv.wait()'s predicate recheck (from Day 2) is not optional here --
// it's the only thing that makes "wake up, but maybe there's nothing to
// do" safe.
//
// You're also tracking a shared "processed count" -- this is Day 1's
// lesson again (protect shared state, lock per-access) stacked on top of
// Day 3's queue. Nothing new conceptually. Today is a stress test of
// what you already understand, not a new primitive.
 
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <atomic>
 
std::queue<int> q;
std::mutex mtx;
std::condition_variable cv;
std::mutex print_mtx;
 
//declare a counter for total items processed, and a mutex to
std::atomic<int> tot_itm_processed = 0;
 
 
// PRODUCER: each of the 4 producer threads pushes 10 ints.
// 'producer_id' just lets you label output so you can tell threads apart.
void producer(int producer_id) {
    for (int i = 0; i < 10; i++) {
        {
            //lock mtx, push a value onto q
            std::lock_guard<std::mutex> plock(mtx);
            q.push(producer_id * 100 + i);
        }
        //multiple consumers hence we need to notify all the consumers;
        cv.notify_one();
    }
}
 
// CONSUMER: pops items until the shared processed-count reaches 40 total
// across ALL consumers combined (not 10 each -- 4 consumers race for 40
// total items, however that splits between them).
void consumer(int consumer_id) {
    while (true) {
        int value;
        {
             // TODO 4: unique_lock on mtx, cv.wait on "!q.empty()"
            std::unique_lock<std::mutex> consLock(mtx);
            cv.wait(consLock, []{ return !q.empty() || tot_itm_processed >= 40; });

            if(q.empty()){
                break;
            }
            else{
                //pop the front value while still holding the lock
                value = q.front();
                q.pop();
                tot_itm_processed++;
            }
            
        }
       

        // lock from TODO 1), and print which consumer processed what.
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
 
    // TODO 8: launch 4 producer threads, passing producer_id 0..3
    for (int i = 0; i < 4; i++) {
        producers.push_back(std::thread(producer, i));
    }
 
    // TODO 9: launch 4 consumer threads, passing consumer_id 0..3
    for (int i = 0; i < 4; i++) {
        consumers.push_back(std::thread(consumer, i));
    }
 
    // TODO 10: join all producers, then join all consumers
    for(int i = 0; i<4; i++){
        producers[i].join();
    }

    for(int i = 0; i<4; i++){
        consumers[i].join();
    }
 
    std::cout << "main: done" << std::endl;
    return 0;
}
 