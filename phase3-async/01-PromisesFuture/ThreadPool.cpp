// CAPSTONE -- THREAD POOL

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <memory>

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads) {
        // spawn num_threads worker threads, each running the
        for(int i = 0; i< num_threads; i++){
            workers_.emplace_back([this] { worker_loop(); });
        }
    }

    ~ThreadPool() {
        shutdown();
    }

    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        using return_type = decltype(f(args...));

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> result = task->get_future();
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            tasks_.emplace([task] { (*task)(); });
        }
        // notify ONE waiting worker that a task is available
        cv_.notify_one();

        // return result;
        return result;
    }

    void shutdown() {
        {
            // lock queue_mutex_, set done_ = true
            std::lock_guard<std::mutex> lock(queue_mutex_);
            if(shutdown_called_) return;
            shutdown_called_ = true;
            done_ = true;
        }
        // notify_all (wake every worker so they can see done_
        // and exit once the queue drains)
        cv_.notify_all();

        // join all worker threads
        for(auto& worker : workers_){
            worker.join();
        }

    }

private:
    void worker_loop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                cv_.wait(lock, [this] {return !tasks_.empty() || done_;});
                //pop the front task into 'task'

                if(done_ && tasks_.empty()){
                    return;
                }
                //
                task = tasks_.front();
                tasks_.pop();
            }
            task();
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    bool done_ = false;
    bool shutdown_called_ = false;
};

// ---- test harness ----
int slow_square(int x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return x * x;
}

int main() {
    ThreadPool pool(4);

    std::vector<std::future<int>> results;
    for (int i = 1; i <= 8; i++) {
        results.push_back(pool.submit(slow_square, i));
    }

    for (int i = 0; i < 8; i++) {
        std::cout << "result " << (i + 1) << " = " << results[i].get() << std::endl;
    }

    pool.shutdown();
    std::cout << "all done" << std::endl;
    return 0;
}