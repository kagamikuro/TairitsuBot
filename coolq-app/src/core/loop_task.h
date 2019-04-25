#pragma once

#include <thread>
#include <chrono>
#include <condition_variable>
#include <atomic>
#include <mutex>

class LoopTask
{
private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic_bool active_ = false;
    std::chrono::duration<int> loop_period_;
    std::thread worker_thread_;
    void worker_loop();
protected:
    std::string description_;
    virtual void work() = 0;
public:
    explicit LoopTask(const int period) : loop_period_(period) {}
    virtual ~LoopTask() { terminate_work(); }
    virtual const char* task_name() = 0;
    const std::string& description() const { return description_; }
    bool is_active() const { return active_; }
    void load_description();
    virtual void save_data() const {}
    virtual void load_data() {}
    void start_work();
    void terminate_work();
};
