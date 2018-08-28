#pragma once

#include <vector>
#include <utility>

template<typename T>
class HashDictionary
{
private:
    int size = 0;
    std::vector<std::vector<std::pair<int64_t, T>>> data;
public:
    HashDictionary() = default;
    explicit HashDictionary(int size) :size(size) { data = std::vector<std::vector<std::pair<int64_t, T>>>(size); }
    void insert(int64_t id, T value) { data[id % size].push_back(std::pair<int64_t, T>(id, value)); }
    bool contains(const int64_t id)
    {
        const int count = data[id % size].size();
        for (int i = 0; i < count; i++)
            if (data[id % size][i].first == id)
                return true;
        return false;
    }
    T& get_value(const int64_t id)
    {
        const int count = data[id % size].size();
        for (int i = 0; i < count; i++)
            if (data[id % size][i].first == id)
                return data[id % size][i].second;
        throw std::exception("The key doesn't exist");
    }
    void clear() { for (int i = 0; i < data.size(); i++) data[i].clear(); }
};
