#pragma once

#include "concurrency/guarded_container.h"

struct UserData final
{
    float score = 0.0f;
};

inline con::UnorderedMap<int64_t, UserData> user_data;
