
#include<vector>
#include<thread>
#include<iostream>
#include<mutex>

int counter = 0;
std::mutex lock_guard;

void increment(){ 
      
      for(int i = 0; i< 100000; i++){
        lock_guard.lock();
        counter++;
        lock_guard.unlock();
      }
      
}

int main(){
    std::vector<std::thread> task;

    //creating 10 threads
    for(int i = 0; i<10; i++){
        task.push_back(std::thread(increment));
    }

    for(auto &t: task){
        t.join();
    }

    std::cout<< "Final counter: " << counter << std::endl;
}
