#pragma once

#include <string>

#include "../../utility/dictionary.h"

class RepeatChain
{
private:
    int length = 0;
    std::string last_message = "";
    Dictionary<int> repeats;
public:
    RepeatChain() = default;
    int new_message(int64_t id, const std::string& message);
    int get_length() const { return length; }
};
