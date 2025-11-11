#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <future>
#include <functional>
#include <mutex>
#include <condition_variable>


class ThreadPool {
    private:
        std::vector<std::thread> worker_threads;
        std::queue<std::function<void()>> tasks;

        std::mutex mutex;
        std::condition_variable cv;
        
        bool stop;
        void worker();

    public:
        ThreadPool(size_t count_threads = std::thread::hardware_concurrency());
        ~ThreadPool();

        template<typename F, typename... Args>
        inline auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
            auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
            auto encapsulated_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

            std::future<std::result_of_t<F(Args...)>> future_object = encapsulated_ptr->get_future();
            {
                std::unique_lock<std::mutex> lock(mutex);
                tasks.emplace([encapsulated_ptr]() {
                    (*encapsulated_ptr)(); // execute the fx
                    });
            }
            cv.notify_one();
            return future_object;
        }

        ThreadPool(ThreadPool&) = delete;
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
};
#endif