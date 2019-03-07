#pragma once

#include "core/task.h"
#include "core/singleton.h"

class DiceRoll final :
    public Task, public Singleton<DiceRoll>
{
private:
    static bool process_string(const std::string& msg, const boost::regex& regex, int64_t& count, int64_t& facade);
    static std::string roll_dice(int64_t count, int64_t facade);
protected:
    bool on_group_msg(int64_t group, int64_t user, const std::string& msg) override;
    bool on_private_msg(int64_t user, const std::string& msg) override;
public:
    const char* task_name() override { return "DiceRoll"; }
};
