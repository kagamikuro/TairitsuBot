#pragma once

#include <stdexcept>

template <typename T>
class Singleton // Use CTAD pattern
{
protected:
    inline static Singleton* instance_ptr_ = nullptr;
    virtual ~Singleton() = default;
public:
    Singleton()
    {
        if (instance_ptr_ != nullptr)
            throw std::runtime_error("Unexpected multiple instances of a singleton class");
        instance_ptr_ = this;
    }
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = default;
    Singleton& operator=(Singleton&&) = default;
    static T& instance()
    {
        if (instance_ptr_ == nullptr)
            throw std::runtime_error("Instance is not yet constructed");
        static T* const ptr = dynamic_cast<T*>(instance_ptr_);
        return *ptr;
    }
};
