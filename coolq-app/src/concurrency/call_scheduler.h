#pragma once

#include <unordered_map>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>

namespace con
{
    class ScheduledCall;

    enum class CallStatus { Done, Interrupted, NoThread };

    struct CvBlock final { std::mutex mutex; std::condition_variable cv; };
    using MutexPtr = std::unique_ptr<std::mutex>;
    using CvPtr = std::unique_ptr<CvBlock>;
    using Call = std::function<void()>;

    class ScheduledCall final
    {
    private:
        CvPtr cv_ptr_ = std::make_unique<CvBlock>();
        bool is_interrupted_ = true;
        Call func_;
        Call destroyer_;
        std::exception_ptr eptr_;
        std::chrono::duration<int> time_limit_{};
        std::thread thread_;
        void work();
    public:
        ~ScheduledCall() noexcept;
        void start(Call func, Call destroyer, int time_limit_seconds);
        CallStatus stop();
    };

    class CallScheduler final
    {
    private:
        bool stop_ = false;
        MutexPtr calls_mutex_ptr_ = std::make_unique<std::mutex>();
        CvPtr queue_cv_ptr_ = std::make_unique<CvBlock>();
        std::thread worker_;
        std::unordered_map<int64_t, ScheduledCall> calls_;
        std::deque<int64_t> destroy_queue_;
        void consume();
    public:
        CallScheduler() = default;
        ~CallScheduler() noexcept;
        CallScheduler(const CallScheduler&) = delete;
        CallScheduler(CallScheduler&&) = default;
        CallScheduler& operator=(const CallScheduler&) = delete;
        CallScheduler& operator=(CallScheduler&&) = default;
        void start();
        void schedule_call(int64_t id, Call func, int time_limit_seconds);
        CallStatus interrupt(int64_t id);
    };
}
