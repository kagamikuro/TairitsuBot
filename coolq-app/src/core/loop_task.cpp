#include "loop_task.h"
#include "utils/utils.h"
#include "../tasks/save_load_manager.h"

void LoopTask::worker_loop()
{
    std::unique_lock lock(mutex_);
    while (cv_.wait_for(lock, loop_period_) == std::cv_status::timeout)
    {
        try { work(); }
        catch (...) { utils::log_exception(u8"任务"s.append(task_name())); }
    }
    active_ = false;
}

void LoopTask::load_description()
{
    const nlohmann::json& strings_json = SaveLoadManager::instance().get_strings();
    if (const auto iter = strings_json.find(task_name()); iter != strings_json.end())
    {
        if (const auto desc_iter = iter->find("description"); desc_iter != iter->end())
            description_ = *desc_iter;
    }
}

void LoopTask::start_work()
{
    {
        std::scoped_lock lock(mutex_);
        if (active_) return;
        active_ = true;
    }
    worker_thread_ = std::thread(&LoopTask::worker_loop, this);
}

void LoopTask::terminate_work()
{
    {
        std::scoped_lock lock(mutex_);
        if (!active_) return;
    }
    cv_.notify_all();
    worker_thread_.join();
}
