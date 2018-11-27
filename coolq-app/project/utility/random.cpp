#include "random.h"

#include <mutex>

namespace random
{
    namespace
    {
        std::mt19937 generator{ std::random_device{}() };
        std::mutex mutex;
    }

    int get_uniform_integer(const int size)
    {
        std::lock_guard lock(mutex);
        const std::uniform_int_distribution<> distribution(0, size - 1);
        return distribution(generator);
    }

    int get_bernoulli(const double p)
    {
        std::lock_guard lock(mutex);
        const std::bernoulli_distribution distribution(p);
        return distribution(generator);
    }
}
