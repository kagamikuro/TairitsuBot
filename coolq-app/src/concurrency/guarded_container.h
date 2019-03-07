#pragma once

#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>

#include "guarded_data.h"

namespace con
{
    template <typename... Types> using Vector = GuardedData<std::vector<Types...>>;
    template <typename... Types> using List = GuardedData<std::list<Types...>>;
    template <typename... Types> using UnorderedMap = GuardedData<std::unordered_map<Types...>>;
    template <typename... Types> using UnorderedSet = GuardedData<std::unordered_set<Types...>>;
}
