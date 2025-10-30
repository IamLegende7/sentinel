#include "thread_pool.hpp"
#include "logger.hpp"

ThreadPool::ThreadPool(std::size_t count_threads) {
    int count_threads_int = count_threads;
    if (count_threads_int == -1) {
        count_threads_int = std::thread::hardware_concurrency();
        LOGGER.log(LogLevel::DEBUG, "[utils/thread_pool.cpp:ThreadPool] Using max threads: %d", count_threads_int);
    }
    stop = false;
    for (int i{ 0 }; i < count_threads_int; i++) {
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