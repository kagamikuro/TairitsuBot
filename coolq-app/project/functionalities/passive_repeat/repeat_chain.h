#pragma once

#include <string>

#include "../../utility/hash_dictionary.h"

class RepeatChain
{
private:
    int length = 0;
    std::string last_message = "";
    HashDictionary<int> repeats;
public:
    RepeatChain();
    int new_message(int64_t id, const std::string& message);
    int get_length() const { return length; }
};
