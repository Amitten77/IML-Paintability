#include "thread_pool.h"

ThreadPool::ThreadPool(size_t numThreads) : stop_(false) {
    for (size_t i = 0; i < numThreads; i++) {
        this->workers_.emplace_back(
            [this]() {
                while (true) {
                    std::function<void()> task;

                    {
                        // Wait for a task to be added to the queue
                        std::unique_lock<std::mutex> lock(this->queueMutex_);
                        this->condition_.wait(lock, [this] {
                            return this->stop_ || !this->tasks_.empty();
                        });

                        // If already stopped and no more tasks, return
                        if (this->stop_ && this->tasks_.empty()) {
                            return;
                        }

                        // Otherwise take the task from the queue
                        task = std::move(this->tasks_.front());
                        this->tasks_.pop();
                    }

                    // Execute the task
                    task();
                }
            }
        );
    }
}

ThreadPool::~ThreadPool() {
    // Stop accepting more threads
    {
        std::unique_lock<std::mutex> lock(this->queueMutex_);
        this->stop_ = true;
    }

    // Tell all threads to stop
    this->condition_.notify_all();
    for (std::thread &worker : this->workers_) {
        worker.join();
    }
}
