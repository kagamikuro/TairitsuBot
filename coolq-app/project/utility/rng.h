#pragma once

#include <random>

class RNG
{
private:
    std::mt19937 generator;
    std::uniform_int_distribution<> distribution;
public:
    RNG() :generator(std::random_device()()) {}
    void set_size(const int size) { distribution = std::uniform_int_distribution<>(0, size - 1); }
    int get_next() { return distribution(generator); }
    int get_random_integer(const int size) { return std::uniform_int_distribution<>(0, size - 1)(generator); }
};
