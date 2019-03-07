#pragma once

#include <string>
#include <unordered_map>

class RepeatChain final
{
public:
    struct RepeatInfo final { int chain_length = 0; int user_repeat_time = 0; };
private:
    int chain_length_ = 0;
    std::string last_msg_;
    std::unordered_map<int64_t, int> repeat_times_;
public:
    RepeatInfo add_new_message(const int64_t user, const std::string& msg)
    {
        if (msg != last_msg_)
        {
            last_msg_ = msg;
            chain_length_ = 1;
            repeat_times_.clear();
            repeat_times_[user] = 1;
            return { chain_length_, 1 };
        }
        return { ++chain_length_, ++repeat_times_[user] };
    }
};
