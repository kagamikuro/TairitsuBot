#include "synchronizer.h"

Synchronizer::Status Synchronizer::consume()
{
    UniqueLock lock(mutex);
    while (queue.empty() && !terminating) condition_variable.wait(lock);
    if (terminating) return Status::Terminated;
    Task& task = queue.front();
    task.second();
    task.first.set_value();
    queue.pop();
    return Status::Continue;
}

void Synchronizer::start()
{
    {
        Lock lock(mutex);
        terminating = false;
    }
    worker_thread = std::thread([this]
    {
        Status status = Status::Continue;
        while (status != Status::Terminated) status = consume();
    });
}

void Synchronizer::terminate()
{
    Lock lock(mutex);
    terminating = true;
    condition_variable.notify_one();
    worker_thread.join();
}

std::future<void> Synchronizer::produce(const Func& work)
{
    Lock lock(mutex);
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    const bool wake = queue.empty();
    queue.emplace(std::move(promise), work);
    if (wake) condition_variable.notify_one();
    return future;
}
