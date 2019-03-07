#pragma once

#include <memory>

#include "singleton.h"
#include "task_manager.h"

class AppManager final : public Singleton<AppManager>
{
private:
    std::unique_ptr<TaskManager> tm_ptr_;
public:
    void initialize();
    void clean_up();
};
