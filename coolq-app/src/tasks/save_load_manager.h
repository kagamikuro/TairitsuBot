#pragma once

#include <nlohmann/json.hpp>

#include "core/loop_task.h"
#include "core/singleton.h"
#include "utils/utils.h"

class SaveLoadManager final :
    public LoopTask, public Singleton<SaveLoadManager>
{
private:
    mutable std::mutex file_io_mutex_;
    const std::string task_status_path_ = utils::data_path + "task_status";
    const std::string strings_path_ = utils::data_path + "strings.json";
    nlohmann::json strings_json_;
protected:
    void work() override { save_all(); }
public:
    SaveLoadManager() : LoopTask(900) {}
    const char* task_name() override { return "SaveLoadManager"; }
    const nlohmann::json& get_strings() const { return strings_json_; }
    void save_all() const;
    void load_all();
};
