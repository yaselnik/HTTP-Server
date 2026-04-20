#include "server/thread_pool.hpp"
#include <iostream>

namespace server{
    ThreadPool::ThreadPool(size_t threads) {
        for (auto i = 0; i < threads; ++i) {
            workers.emplace_back([this] {worker_loop();});
        }
    }

    ThreadPool::~ThreadPool() {
        stop = true;
        condition.notify_all();
    }

    void ThreadPool::worker_loop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock lock(queue_mutex);
                condition.wait(lock, [this] { return stop || !tasks.empty(); });
                if (stop && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();
            }

            task();
        }
    }
}
