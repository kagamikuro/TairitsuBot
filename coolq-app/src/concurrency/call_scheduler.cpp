#include "call_scheduler.h"
#include "utils/utils.h"

namespace con
{
    using namespace std::literals;

    void ScheduledCall::work() try
    {
        {
            std::unique_lock lock(cv_ptr_->mutex);
            if (cv_ptr_->cv.wait_for(lock, time_limit_) == std::cv_status::timeout)
            {
                is_interrupted_ = false;
                func_();
            }
        }
        destroyer_();
    }
    catch (...) { eptr_ = std::current_exception(); }

    ScheduledCall::~ScheduledCall() noexcept
    {
        if (thread_.joinable())
        {
            {
                std::unique_lock lock(cv_ptr_->mutex);
                cv_ptr_->cv.notify_one();
            }
            thread_.join();
        }
    }

    void ScheduledCall::start(Call func, Call destroyer, const int time_limit_seconds)
    {
        if (thread_.joinable())
            throw std::runtime_error("ScheduledCall::start: Thread is running");
        time_limit_ = std::chrono::duration<int>(time_limit_seconds);
        func_ = std::move(func);
        destroyer_ = std::move(destroyer);
        thread_ = std::thread(&ScheduledCall::work, this);
    }

    CallStatus ScheduledCall::stop()
    {
        if (!thread_.joinable()) return CallStatus::NoThread;
        {
            std::unique_lock lock(cv_ptr_->mutex);
            cv_ptr_->cv.notify_one();
        }
        thread_.join();
        if (eptr_) std::rethrow_exception(eptr_);
        return is_interrupted_ ? CallStatus::Interrupted : CallStatus::Done;
    }

    void CallScheduler::consume()
    {
        std::unique_lock queue_lock(queue_cv_ptr_->mutex);
        std::deque<int64_t> local_queue;
        while (true)
        {
            queue_cv_ptr_->cv.wait_for(queue_lock, 10s);
            if (stop_) return;
            if (destroy_queue_.empty()) continue;
            while (!destroy_queue_.empty())
            {
                local_queue.push_back(destroy_queue_.front());
                destroy_queue_.pop_front();
            }
            queue_lock.unlock();
            {
                std::unique_lock call_lock(*calls_mutex_ptr_);
                while (!local_queue.empty())
                {
                    calls_.erase(local_queue.front());
                    local_queue.pop_front();
                }
            }
            queue_lock.lock();
        }
    }

    CallScheduler::~CallScheduler() noexcept
    {
        if (!worker_.joinable()) return;
        {
            std::unique_lock lock(queue_cv_ptr_->mutex);
            stop_ = true;
            queue_cv_ptr_->cv.notify_one();
        }
        worker_.join();
    }

    void CallScheduler::start()
    {
        if (worker_.joinable()) return;
        worker_ = std::thread(&CallScheduler::consume, this);
    }

    void CallScheduler::schedule_call(const int64_t id, Call func, const int time_limit_seconds)
    {
        std::unique_lock call_lock(*calls_mutex_ptr_);
        calls_[id].start(std::move(func), [id, this]
        {
            std::unique_lock queue_lock(queue_cv_ptr_->mutex);
            const bool empty = destroy_queue_.empty();
            destroy_queue_.push_back(id);
            if (empty) queue_cv_ptr_->cv.notify_one();
        }, time_limit_seconds);
    }

    CallStatus CallScheduler::interrupt(const int64_t id)
    {
        std::unique_lock call_lock(*calls_mutex_ptr_);
        if (calls_.find(id) == calls_.end()) return CallStatus::NoThread;
        return calls_[id].stop();
    }
}
