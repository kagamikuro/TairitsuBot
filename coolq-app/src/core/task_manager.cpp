#include "task_manager.h"

Task* TaskManager::find_task(const std::string& name)
{
    using Iter = std::vector<std::unique_ptr<Task>>::const_iterator;
    const Iter iter = std::find_if(tasks_.begin(), tasks_.end(),
        [&name](const std::unique_ptr<Task>& task) { return name == task->task_name(); });
    if (iter == tasks_.end()) return nullptr;
    return iter->get();
}

LoopTask* TaskManager::find_loop_task(const std::string& name)
{
    using Iter = std::vector<std::unique_ptr<LoopTask>>::const_iterator;
    const Iter iter = std::find_if(loop_tasks_.begin(), loop_tasks_.end(),
        [&name](const std::unique_ptr<LoopTask>& task) { return name == task->task_name(); });
    if (iter == loop_tasks_.end()) return nullptr;
    return iter->get();
}

void TaskManager::clear_task()
{
    for (const std::unique_ptr<Task>& task : tasks_) task->save_data();
    for (const std::unique_ptr<LoopTask>& task : loop_tasks_) task->save_data();
    tasks_.clear();
    loop_tasks_.clear();
}

void TaskManager::on_msg_receive(const cq::Target& target, const std::string& msg)
{
    for (const std::unique_ptr<Task>& task : tasks_)
        if (task->on_msg_receive(target, msg))
            return;
}
