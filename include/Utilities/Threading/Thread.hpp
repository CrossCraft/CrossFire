#pragma once
#include <functional>
#include <thread>
#include "../Types.hpp"
#include "../Allocator.hpp"
#include "Utilities/List.hpp"
#include "Utilities/TailQueue.hpp"
#include <atomic>
#include <condition_variable>
#include <queue>
#include <future>
#include <tuple>

namespace CrossFire
{

/**
 * @brief A simple thread class.
 */
class Thread {
    std::thread thread;
    std::function<void()> function;

public:
    /**
     * @brief Construct a new Thread object.
     * @tparam F The type of the function.
     * @param f The function.
     */
    template <typename F>
    explicit Thread(F &&f)
        : function(std::forward<F>(f))
    {
    }

    /**
     * @brief Start the thread.
     * @tparam Args The types of the arguments.
     * @param args The arguments.
     */
    template <typename... Args> auto start(Args &&...args) -> void
    {
        std::tuple<> args_tuple = std::make_tuple(std::forward<Args>(args)...);
        thread = std::thread(
            [this, args_tuple]() { std::apply(function, args_tuple); });
    }

    /**
     * @brief Join the thread.
     */
    auto join() -> void
    {
        thread.join();
    }
};

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads)
        : stop(false)
    {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] {
                for (;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex);
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                        });
                        if (this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template <class F, class... Args>
    auto enqueue(F &&f, Args &&...args)
        -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");
            tasks.emplace([task] { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers) {
            worker.join();
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()> > tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic_bool stop;
};

}