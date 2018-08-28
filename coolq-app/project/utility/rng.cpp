#include "rng.h"

RNG::RNG() { generator = std::mt19937(std::random_device()()); }

void RNG::set_size(const int size) { distribution = std::uniform_int_distribution<>(0, size - 1); }

int RNG::get_next() { return distribution(generator); }

int RNG::get_random_integer(const int size)
{
    set_size(size);
    return get_next();
}
