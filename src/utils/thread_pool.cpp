#include "utils/thread_pool.hpp"
#include "utils/logger.hpp"

ThreadPool::ThreadPool(int count_threads) {
    if (count_threads == -1) {
        count_threads = std::thread::hardware_concurrency();
        LOGGER.log(LogLevel::DEBUG, "[utils/thread_pool.cpp:ThreadPool] Using max threads: %d", count_threads);
    }
    stop = false;
    for (int i{ 0 }; i < count_threads; i++) {
        worker_threads.emplace_back(&ThreadPool::worker, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        stop = true;
    }

    cv.notify_all();
    for (auto& worker : worker_threads) {
        worker.join();
    }
}


void ThreadPool::worker() {
    for (;;) {
        std::function<void()> cur_task;
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this] { return stop || !tasks.empty(); });

            if (stop && tasks.empty()) 
                break;
            if (tasks.empty())
                continue; 

            cur_task = tasks.front();
            tasks.pop();
        }
        cur_task();
    }
}