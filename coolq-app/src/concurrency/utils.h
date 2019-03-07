#pragma once

#include <mutex>
#include <chrono>

namespace con::this_thread
{
    using SteadyClock = std::chrono::steady_clock;

    namespace detail
    {
        inline std::mutex sleep_mutex;
        inline std::condition_variable sleep_cv;
        inline bool interrupted = false;
    }

    // Sleep for a certain duration, or until interrupted
    // Returns whether the sleep is interrupted
    template <typename Rep, typename Period>
    bool sleep_for(const std::chrono::duration<Rep, Period>& sleep_duration)
    {
        using namespace detail;
        std::unique_lock lock(sleep_mutex);
        return sleep_cv.wait_for(lock, sleep_duration, [] { return interrupted; });
    }

    // Sleep until a certain time point, or until interrupted
    // Returns whether the sleep is interrupted
    template <typename Duration>
    bool sleep_until(const std::chrono::time_point<SteadyClock, Duration>& sleep_time)
    {
        using namespace detail;
        std::unique_lock lock(sleep_mutex);
        return sleep_cv.wait_until(lock, sleep_time, [] { return interrupted; });
    }

    // Interrupt all sleeping threads
    inline void interrupt()
    {
        using namespace detail;
        {
            std::unique_lock lock(sleep_mutex);
            interrupted = true;
        }
        sleep_cv.notify_all();
    }
}
