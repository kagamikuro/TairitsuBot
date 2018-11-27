#pragma once

#include <random>

namespace random
{
    int get_uniform_integer(int size);
    int get_bernoulli(double p = 0.5);
}
