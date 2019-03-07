#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "singleton.h"
#include "task.h"
#include "loop_task.h"
#include "cqsdk/message.h"

class TaskManager final : public Singleton<TaskManager>
{
private:
    template <typename T>
    using Action = std::function<void(T)>;
    std::vector<std::unique_ptr<Task>> tasks_;
    std::vector<std::unique_ptr<LoopTask>> loop_tasks_;
public:
    template <typename T> void add_task()
    {
        std::unique_ptr<Task>& task = tasks_.emplace_back(std::make_unique<T>());
        task->load_data();
    }
    template <typename T> void add_loop_task()
    {
        std::unique_ptr<LoopTask>& task = loop_tasks_.emplace_back(std::make_unique<T>());
        task->load_data();
    }
    void for_each_task(Action<Task&>&& func) { for (const std::unique_ptr<Task>& task : tasks_) func(*task); }
    void for_each_loop_task(Action<LoopTask&>&& func) { for (const std::unique_ptr<LoopTask>& task : loop_tasks_) func(*task); }
    Task* find_task(const std::string& name);
    LoopTask* find_loop_task(const std::string& name);
    void clear_task();
    void on_msg_receive(const cq::Target& target, const std::string& msg);
};
