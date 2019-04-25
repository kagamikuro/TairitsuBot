#include <random>
#include <mutex>

#include "random.h"

namespace utils
{
    namespace
    {
        std::mt19937 gen{ std::random_device{ }() };
        std::mutex mutex;
    }

    int32_t random_uniform_int(const int32_t a, const int32_t b)
    {
        const std::uniform_int_distribution<int32_t> dist(a, b);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    float random_uniform_float(const float a, const float b)
    {
        const std::uniform_real_distribution<float> dist(a, b);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    bool random_bernoulli_bool(const float p)
    {
        const std::bernoulli_distribution dist(p);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    int32_t random_binomial_int(const int32_t n, const float p)
    {
        const std::binomial_distribution<int32_t> dist(n, p);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    int32_t random_negative_binomial_int(const int32_t r, const float p)
    {
        const std::negative_binomial_distribution<int> dist(r, p);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    int32_t random_geometric_int(const float p)
    {
        const std::geometric_distribution<int> dist(p);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    int32_t random_poisson_int(const float lambda)
    {
        const std::poisson_distribution<int32_t> dist(lambda);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    float random_exponential_float(const float lambda)
    {
        const std::exponential_distribution<float> dist(lambda);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    float random_gamma_float(const float k, const float theta)
    {
        const std::gamma_distribution<float> dist(k, theta);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    float random_weibull_float(const float k, const float lambda)
    {
        const std::weibull_distribution<float> dist(k, lambda);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    float random_extreme_value_float(const float mu, const float sigma)
    {
        const std::extreme_value_distribution<float> dist(mu, sigma);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    float random_normal_float(const float mu, const float sigma)
    {
        std::normal_distribution<float> dist(mu, sigma);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    float random_lognormal_float(const float mu, const float sigma)
    {
        const std::lognormal_distribution<float> dist(mu, sigma);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    float random_chi_squared_float(const int32_t k)
    {
        const std::chi_squared_distribution<float> dist{ float(k) };
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    float random_cauchy_float(const float x0, const float gamma)
    {
        const std::cauchy_distribution<float> dist(x0, gamma);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    float random_fisher_f_float(const int32_t d1, const int32_t d2)
    {
        const std::fisher_f_distribution<float> dist{ float(d1), float(d2) };
        std::scoped_lock lock(mutex);
        return dist(gen);
    }

    float random_student_t_float(const float nu)
    {
        const std::student_t_distribution<float> dist(nu);
        std::scoped_lock lock(mutex);
        return dist(gen);
    }
}
