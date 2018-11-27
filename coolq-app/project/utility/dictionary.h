#pragma once

#include <map>
#include <mutex>

template<typename T>
class Dictionary
{
    using Lock = std::lock_guard<std::recursive_mutex>;
private:
    std::map<int64_t, T> data;
    mutable std::recursive_mutex mutex;
public:
    Dictionary() = default;
    bool contains(const int64_t key)
    {
        Lock lock(mutex);
        return data.find(key) != data.end();
    }
    // Get the value, construct if needed
    // Actually if T is a large struct or class, consider using manipulate instead
    T get(const int64_t key)
    {
        Lock lock(mutex);
        return data[key];
    }
    // Set or insert a value
    // Getter and setter are used instead of the operator[] which returns a reference,
    // since all of the operations should be thread safe. Returning references may lead to
    // dangling references if used in multi-thread environment. Since I only use this class
    // in a multi-thread environment, an operator[] described above is of no use.
    void set(const int64_t key, const T& value)
    {
        Lock lock(mutex);
        data[key] = value;
    }
    // Same as the method above, but for a movable T.
    void set(const int64_t key, T&& value)
    {
        Lock lock(mutex);
        data[key] = value;
    }
    void remove(const int64_t key)
    {
        Lock lock(mutex);
        typename std::map<int64_t, T>::iterator match = data.find(key);
        if (match == data.end()) throw std::exception("Dictionary<T>::remove: Key not found.");
        data.erase(match);
    }
    void clear()
    {
        Lock lock(mutex);
        data.clear();
    }
    // Do something synchronously with the found value
    template <typename Func>
    auto manipulate(const int64_t key, const Func& function)
    {
        Lock lock(mutex);
        return function(data[key]);
    }
};
