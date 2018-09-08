#pragma once

#include <vector>
#include <memory>
#include "processing/message_received.h"
#include "processing/loop_task.h"

extern std::vector<std::unique_ptr<MessageReceived>> group_actions;
extern std::vector<std::unique_ptr<MessageReceived>> private_actions;
extern std::vector<std::unique_ptr<MessageReceived>> message_actions;
extern std::vector<std::unique_ptr<LoopTask>> loop_tasks;
extern std::unique_ptr<MessageReceived> creator_commands;
