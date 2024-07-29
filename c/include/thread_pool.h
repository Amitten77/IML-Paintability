#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);

    ~ThreadPool();

    template<typename F, typename... Args>
    auto addTask(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;

        // Package the task
        auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(this->queueMutex_);

            // Don't allow enqueueing after stopping
            if (this->stop_) {
                throw std::runtime_error("addTask on stopped ThreadPool");
            }

            tasks_.emplace([task](){ (*task)(); });
        }

        // If there are waiting threads, notify one that there is a new task
        this->condition_.notify_one();

        return res;
    }

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    // Mutex bind to the queue
    std::mutex queueMutex_;

    // Condition variable to notify the workers
    std::condition_variable condition_;

    // Whether the pool is stopped and no more tasks can be added
    bool stop_;
};

#endif // THREAD_POOL_H
