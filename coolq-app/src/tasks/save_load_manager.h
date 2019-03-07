#pragma once

#include "core/loop_task.h"
#include "core/singleton.h"
#include "utils/utils.h"

class SaveLoadManager final :
    public LoopTask, public Singleton<SaveLoadManager>
{
private:
    mutable std::mutex file_io_mutex_;
    const std::string task_status_path_ = utils::data_path + "task_status";
protected:
    void work() override { save_all(); }
public:
    SaveLoadManager() :LoopTask(900) {}
    const char* task_name() override { return "SaveLoadManager"; }
    void save_all() const;
    void load_all() const;
};
