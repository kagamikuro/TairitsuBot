#include <fstream>

#include "save_load_manager.h"
#include "core/task_manager.h"
#include "utils/binary_io.h"

void SaveLoadManager::save_all() const
{
    TaskManager& tm = TaskManager::instance();
    std::scoped_lock lock(file_io_mutex_);
    std::ofstream stream(task_status_path_, std::ios::out | std::ios::binary);
    tm.for_each_task([&stream](Task& task)
    {
        utils::write(stream, task.task_name());
        utils::write(stream, task.is_active());
        utils::write(stream, task.group_whitelist());
        utils::write(stream, task.user_blacklist());
        task.save_data();
    });
    tm.for_each_loop_task([&stream](LoopTask& task)
    {
        utils::write(stream, task.task_name());
        utils::write(stream, task.is_active());
        task.save_data();
    });
}

void SaveLoadManager::load_all() const
{
    TaskManager& tm = TaskManager::instance();
    std::scoped_lock lock(file_io_mutex_);
    std::ifstream stream(task_status_path_, std::ios::in | std::ios::binary);
    if (!stream.good()) return;
    tm.for_each_task([](Task& task) { task.load_data(); });
    tm.for_each_loop_task([](LoopTask& task) { task.load_data(); });
    while (true)
    {
        const std::string task_name = utils::read<std::string>(stream);
        if (task_name.empty()) break;
        if (Task* const task_ptr = tm.find_task(task_name); task_ptr)
        {
            task_ptr->set_active(utils::read<bool>(stream));
            task_ptr->group_whitelist() = utils::read<con::UnorderedSet<int64_t>>(stream);
            task_ptr->user_blacklist() = utils::read<con::UnorderedSet<int64_t>>(stream);
            continue;
        }
        if (LoopTask* const task_ptr = tm.find_loop_task(task_name); task_ptr)
        {
            const bool active = utils::read<bool>(stream);
            if (active)
                task_ptr->start_work();
            else
                task_ptr->terminate_work();
            continue;
        }
        utils::send_creator(fmt::format(u8"读取数据时发生了错误：我并没有找到名为{}的任务呢……", task_name));
    }
}
