#include <fstream>

#include "app_manager.h"
#include "tasks/tasks.h"
#include "utils/utils.h"
#include "safety_check/logging.h"
#include "tasks/save_load_manager.h"
#include "concurrency/utils.h"

void AppManager::initialize()
{
    cqc::app::enabled = true;
    cqc::logging::debug(u8"Starts up", u8"Add-on started");
    std::ifstream data_path_reader("path.txt");
    std::getline(data_path_reader, utils::data_path);
    // Initialize tasks
    tm_ptr_ = std::make_unique<TaskManager>();
    TaskManager& tm = *tm_ptr_;
    tm.add_task<CreatorCommands>();
    tm.add_task<BanGroup>();
    tm.add_task<BanMember>();
    tm.add_task<DiceRoll>();
    tm.add_task<RandomSample>();
    tm.add_task<Repeat>();
    tm.add_task<ReportMessage>();
    tm.add_task<Ridicule>();
    tm.add_loop_task<LoopUnbanCreator>();
    tm.add_loop_task<SaveLoadManager>();
    // Load data
    SaveLoadManager::instance().load_all();
}

void AppManager::clean_up()
{
    con::this_thread::interrupt();
    cqc::app::enabled = false;
    SaveLoadManager::instance().save_all();
    tm_ptr_->clear_task();
    tm_ptr_.reset();
}
