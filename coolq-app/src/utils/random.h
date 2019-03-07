#pragma once

namespace utils
{
    int32_t random_uniform_int(int32_t a, int32_t b);
    float random_uniform_float(float a, float b);
    bool random_bernoulli_bool(float p = 0.5);
    int32_t random_binomial_int(int32_t n, float p = 0.5);
    int32_t random_negative_binomial_int(int32_t r, float p = 0.5);
    int32_t random_geometric_int(float p = 0.5);
    int32_t random_poisson_int(float lambda = 1.0f);
    float random_exponential_float(float lambda = 1.0f);
    float random_gamma_float(float k = 1.0f, float theta = 1.0f);
    float random_weibull_float(float k = 1.0f, float lambda = 1.0f);
    float random_extreme_value_float(float mu = 0.0f, float sigma = 1.0f);
    float random_normal_float(float mu = 0.0f, float sigma = 1.0f);
    float random_lognormal_float(float mu = 0.0f, float sigma = 1.0f);
    float random_chi_squared_float(int32_t k = 1);
    float random_cauchy_float(float x0 = 0.0f, float gamma = 1.0f);
    float random_fisher_f_float(int32_t d1 = 1, int32_t d2 = 1);
    float random_student_t_float(float nu = 1.0f);
}
