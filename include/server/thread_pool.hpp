#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>
#include <atomic>

namespace server {
    class ThreadPool {
    public:
        explicit ThreadPool(size_t threads = std::thread::hardware_concurrency());
        ~ThreadPool();

        template<typename F>
        void submit(F&& task);
    private:
        std::vector<std::jthread> workers;
        std::queue<std::function<void()>> tasks;

        std::mutex queue_mutex;
        std::condition_variable condition;
        std::atomic<bool> stop{false};
        void worker_loop();
    };

    template<typename F>
    void ThreadPool::submit(F&& task) {
        {
            std::lock_guard lock(queue_mutex);
            if (stop) return;
            tasks.emplace([task_ptr = std::make_shared<std::decay_t<F>>(std::forward<F>(task))]() {
                (*task_ptr)();
            });
        }

        condition.notify_one();
    }
}
