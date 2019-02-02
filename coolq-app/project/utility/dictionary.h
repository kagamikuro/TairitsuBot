#pragma once

#include <unordered_map>
#include <mutex>

template<typename T>
class Dictionary
{
    using Lock = std::lock_guard<std::recursive_mutex>;
private:
    std::unordered_map<int64_t, T> data_;
    mutable std::recursive_mutex mutex_;
public:
    Dictionary() = default;
    bool contains(const int64_t key)
    {
        Lock lock(mutex_);
        return data_.find(key) != data_.end();
    }
    // Get the value, construct if needed
    // Actually if T is a large struct or class, consider using manipulate instead
    T get(const int64_t key)
    {
        Lock lock(mutex_);
        return data_[key];
    }
    // Set or insert a value
    // Getter and setter are used instead of the operator[] which returns a reference,
    // since all of the operations should be thread safe. Returning references may lead to
    // dangling references if used in multi-thread environment. Since I only use this class
    // in a multi-thread environment, an operator[] described above is of no use.
    void set(const int64_t key, const T& value)
    {
        Lock lock(mutex_);
        data_[key] = value;
    }
    // Same as the method above, but for a movable T.
    void set(const int64_t key, T&& value)
    {
        Lock lock(mutex_);
        data_[key] = value;
    }
    void remove(const int64_t key)
    {
        Lock lock(mutex_);
        auto&& match = data_.find(key);
        if (match == data_.end()) throw std::exception("Dictionary<T>::remove: Key not found.");
        data_.erase(match);
    }
    void clear()
    {
        Lock lock(mutex_);
        data_.clear();
    }
    // Do something synchronously with the found value
    template <typename Func>
    auto manipulate(const int64_t key, const Func& function)
    {
        Lock lock(mutex_);
        return function(data_[key]);
    }
    // Do something synchronously with every value
    template <typename Func>
    auto for_each(const Func& function)
    {
        Lock lock(mutex_);
        for (auto[key, value] : data_) function(key, value);
    }
};
