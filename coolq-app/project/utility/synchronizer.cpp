#include "synchronizer.h"

Synchronizer::Status Synchronizer::consume()
{
    UniqueLock lock(mutex_);
    while (queue_.empty() && !terminating_) condition_variable_.wait(lock);
    if (terminating_) return Status::Terminated;
    Task& task = queue_.front();
    task.second();
    task.first.set_value();
    queue_.pop();
    return Status::Continue;
}

void Synchronizer::start()
{
    {
        Lock lock(mutex_);
        terminating_ = false;
    }
    worker_thread_ = std::thread([this]
    {
        Status status = Status::Continue;
        while (status != Status::Terminated) status = consume();
    });
}

void Synchronizer::terminate()
{
    Lock lock(mutex_);
    terminating_ = true;
    condition_variable_.notify_one();
    worker_thread_.join();
}

std::future<void> Synchronizer::produce(const Func& work)
{
    Lock lock(mutex_);
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    const bool wake = queue_.empty();
    queue_.emplace(std::move(promise), work);
    if (wake) condition_variable_.notify_one();
    return future;
}
