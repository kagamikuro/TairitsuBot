#pragma once

#include <thread>
#include <queue>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

class Synchronizer
{
    using Func = std::function<void()>;
    using Task = std::pair<std::promise<void>, Func>;
    using Lock = std::lock_guard<std::recursive_mutex>;
    using UniqueLock = std::unique_lock<std::recursive_mutex>;
    enum class Status
    {
        Continue,
        Terminated
    };
private:
    std::recursive_mutex mutex;
    std::thread worker_thread;
    std::condition_variable_any condition_variable;
    std::queue<Task> queue;
    std::atomic_bool terminating = false;
    Status consume();
    void start();
    void terminate();
public:
    Synchronizer() = default;
    ~Synchronizer() { terminate(); }
    Synchronizer(Synchronizer&) = delete;
    Synchronizer& operator= (const Synchronizer&) = delete;
    std::future<void> produce(const Func& work);
};
