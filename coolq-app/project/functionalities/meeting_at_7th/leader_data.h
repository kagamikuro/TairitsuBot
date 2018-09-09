#pragma once

#include <string>

struct LeaderData
{
    int64_t user_id;
    std::string name;
    std::string full_title;
    LeaderData(const int64_t user_id, const std::string& name, const std::string& full_title) :
        user_id(user_id), name(name), full_title(full_title) {}
};
