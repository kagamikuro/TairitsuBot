#pragma once

#include <chrono>
#include <thread>

// Using this could be really dangerous and lead to a series of problems
// Do not try this at home
class LoopTask
{
private:
    bool running = false;
    bool terminate = false;
    std::chrono::duration<int> loop_period;
    std::thread loop_thread;
protected:
    virtual void task() = 0;
public:
    explicit LoopTask(const int period = 60) :loop_period(period) { start_loop(); }
    void start_loop();
    void terminate_loop() { terminate = true; }
    void join_thread() { loop_thread.join(); }
    bool is_running() const { return running; }
    virtual ~LoopTask() = default; // BE SURE TO TERMINATE THE THREAD BEFORE RELEASING MEMORY
};
