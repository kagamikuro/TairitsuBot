#pragma once

#include <random>

class RNG
{
private:
    std::mt19937 generator;
    std::uniform_int_distribution<> distribution;
public:
    RNG();
    void set_size(int size);
    int get_next();
    int get_random_integer(int size);
};
