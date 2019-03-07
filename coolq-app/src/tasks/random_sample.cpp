#include "random_sample.h"
#include "utils/random.h"

namespace
{
    std::optional<std::string> check_uniform_int(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"(U(?:nif)? *\{{ *{0}*, *{0} *\}})", utils::int_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<int32_t> low = utils::parse_number<int32_t>(utils::regex_match_to_view(match[1]));
        const std::optional<int32_t> high = utils::parse_number<int32_t>(utils::regex_match_to_view(match[2]));
        if (!low || !high) return u8"你给的数据超出我能处理的范围了……";
        if (*low > *high) return u8"区间的下限是不能比上限还大的啊";
        const int32_t sample = utils::random_uniform_int(*low, *high);
        return fmt::format(u8"从服从离散均匀分布 U{{{}, {}}} 的整体中抽出了样本 {}", *low, *high, sample);
    }

    std::optional<std::string> check_uniform_float(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"(U(?:nif)? *\( *{0} *, *{0} *\))", utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<float> low = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        const std::optional<float> high = utils::parse_number<float>(utils::regex_match_to_view(match[2]));
        if (!low || !high) return u8"你给的数据超出我能处理的范围了……";
        if (*low >= *high) return u8"区间的下限要比上限小的啊";
        const float sample = utils::random_uniform_float(*low, *high);
        return fmt::format(u8"从服从连续均匀分布 U({}, {}) 的整体中抽出了样本 {}", *low, *high, sample);
    }

    std::optional<std::string> check_bernoulli(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"([Bb]ernoulli *\( *{} *\))", utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<float> p = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        if (!p) return u8"你给的数据超出我能处理的范围了……";
        if (*p < 0 || *p > 1) return u8"成功概率需要在[0, 1]区间里哦";
        const bool sample = utils::random_bernoulli_bool(*p);
        return fmt::format(u8"从服从分布 Bernoulli({}) 的整体中抽出了样本 {}", *p, sample);
    }

    std::optional<std::string> check_binomial(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"(B(?:inom)? *\( *{} *, *{} *\))", utils::int_regex, utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<int32_t> n = utils::parse_number<int32_t>(utils::regex_match_to_view(match[1]));
        const std::optional<float> p = utils::parse_number<float>(utils::regex_match_to_view(match[2]));
        if (!n || !p) return u8"你给的数据超出我能处理的范围了……";
        if (n < 0) return u8"试验次数应该是非负的哦";
        if (*p < 0 || *p > 1) return u8"成功概率需要在[0, 1]区间里哦";
        const int32_t sample = utils::random_binomial_int(*n, *p);
        return fmt::format(u8"从服从二项分布 B({}, {}) 的整体中抽出了样本 {}", *n, *p, sample);
    }

    std::optional<std::string> check_negative_binomial(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"(NB *\( *{} *, *{} *\))", utils::int_regex, utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<int32_t> n = utils::parse_number<int32_t>(utils::regex_match_to_view(match[1]));
        const std::optional<float> p = utils::parse_number<float>(utils::regex_match_to_view(match[2]));
        if (!n || !p) return u8"你给的数据超出我能处理的范围了……";
        if (n < 0) return u8"试验次数应该是非负的哦";
        if (*p < 0 || *p > 1) return u8"成功概率需要在[0, 1]区间里哦";
        const int32_t sample = utils::random_binomial_int(*n, *p);
        return fmt::format(u8"从服从负二项分布 NB({}, {}) 的整体中抽出了样本 {}", *n, *p, sample);
    }

    std::optional<std::string> check_geometric(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"(G(?:eo)? *\( *{} *\))", utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<float> p = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        if (!p) return u8"你给的数据超出我能处理的范围了……";
        if (*p < 0 || *p > 1) return u8"成功概率需要在[0, 1]区间里哦";
        const int32_t sample = utils::random_geometric_int(*p);
        return fmt::format(u8"从服从几何分布 G({}) 的整体中抽出了样本 {}", *p, sample);
    }

    std::optional<std::string> check_poisson(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"(Pois(?:son)? *\( *{} *\))", utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<float> lambda = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        if (!lambda) return u8"你给的数据超出我能处理的范围了……";
        if (*lambda <= 0) return u8"单位时间事件的平均发生率应该是正数";
        const int32_t sample = utils::random_poisson_int(*lambda);
        return fmt::format(u8"从服从Poisson分布 Pois({}) 的整体中抽出了样本 {}", *lambda, sample);
    }

    std::optional<std::string> check_exponential(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"([Ee]xp *\( *{} *\))", utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<float> lambda = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        if (!lambda) return u8"你给的数据超出我能处理的范围了……";
        if (*lambda <= 0) return u8"单位时间事件的平均发生率应该是正数";
        const float sample = utils::random_exponential_float(*lambda);
        return fmt::format(u8"从服从指数分布 Exp({}) 的整体中抽出了样本 {}", *lambda, sample);
    }

    std::optional<std::string> check_gamma(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"((?:[Gg]amma|Γ) *\( *{0} *, *{0} *\))", utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<float> k = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        const std::optional<float> theta = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        if (!k || !theta) return u8"你给的数据超出我能处理的范围了……";
        if (*k <= 0) return u8"形状参数需要是正数";
        if (*theta <= 0) return u8"尺度参数需要是正数";
        const float sample = utils::random_gamma_float(*k, *theta);
        return fmt::format(u8"从服从Gamma分布 Γ({}, {}) 的整体中抽出了样本 {}", *k, *theta, sample);
    }

    std::optional<std::string> check_weibull(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"((?:U|unif) *\( *{0} *, *{0} *\))", utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<float> k = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        const std::optional<float> lambda = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        if (!k || !lambda) return u8"你给的数据超出我能处理的范围了……";
        if (*k <= 0) return u8"形状参数需要是正数";
        if (*lambda <= 0) return u8"尺度参数需要是正数";
        const float sample = utils::random_gamma_float(*k, *lambda);
        return fmt::format(u8"从服从分布 Weibull({}, {}) 的整体中抽出了样本 {}", *k, *lambda, sample);
    }

    std::optional<std::string> check_extreme_value(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"(EV *\( *{0} *, *{0} *\))", utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<float> mu = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        const std::optional<float> sigma = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        if (!mu || !sigma) return u8"你给的数据超出我能处理的范围了……";
        if (*sigma <= 0) return u8"尺度参数需要是正数";
        const float sample = utils::random_extreme_value_float(*mu, *sigma);
        return fmt::format(u8"从服从极值分布 EV({}, {}) 的整体中抽出了样本 {}", *mu, *sigma, sample);
    }

    std::optional<std::string> check_normal_float(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"(N(?:orm)? *\( *{0} *, *{0} *\))", utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<float> mu = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        const std::optional<float> sigma_sqr = utils::parse_number<float>(utils::regex_match_to_view(match[2]));
        if (!mu || !sigma_sqr) return u8"你给的数据超出我能处理的范围了……";
        if (*sigma_sqr <= 0.0f) return u8"方差是需要大于0的哦";
        const float sample = utils::random_normal_float(*mu, std::sqrt(*sigma_sqr));
        return fmt::format(u8"从服从正态分布 N({}, {}) 的整体中抽出了样本 {}", *mu, *sigma_sqr, sample);
    }

    std::optional<std::string> check_lognormal_float(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"([Ll]ognorm(?:al)? *\( *{0} *, *{0} *\))", utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<float> mu = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        const std::optional<float> sigma_sqr = utils::parse_number<float>(utils::regex_match_to_view(match[2]));
        if (!mu || !sigma_sqr) return u8"你给的数据超出我能处理的范围了……";
        if (*sigma_sqr <= 0.0f) return u8"方差是需要大于0的哦";
        const float sample = utils::random_lognormal_float(*mu, std::sqrt(*sigma_sqr));
        return fmt::format(u8"从服从对数正态分布 Lognormal({}, {}) 的整体中抽出了样本 {}", *mu, *sigma_sqr, sample);
    }

    std::optional<std::string> check_chi_squared(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"((?:[Cc]hi|χ)(?:\^?2|²) *\( *{} *\))", utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<int32_t> k = utils::parse_number<int32_t>(utils::regex_match_to_view(match[1]));
        if (!k) return u8"你给的数据超出我能处理的范围了……";
        if (*k <= 0) return u8"分布的自由度应该是正整数";
        const float sample = utils::random_chi_squared_float(*k);
        return fmt::format(u8"从服从分布 χ²({}) 的整体中抽出了样本 {}", *k, sample);
    }

    std::optional<std::string> check_cauchy(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"([Cc]auchy *\( *{0} *, *{0} *\))", utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<float> x0 = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        const std::optional<float> gamma = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        if (!x0 || !gamma) return u8"你给的数据超出我能处理的范围了……";
        if (*gamma <= 0) return u8"尺度参数需要是正数";
        const float sample = utils::random_cauchy_float(*x0, *gamma);
        return fmt::format(u8"从服从分布 Cauchy({}, {}) 的整体中抽出了样本 {}", *x0, *gamma, sample);
    }

    std::optional<std::string> check_fisher_f(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"(F *\{{ *{0}*, *{0} *\}})", utils::int_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<int32_t> d1 = utils::parse_number<int32_t>(utils::regex_match_to_view(match[1]));
        const std::optional<int32_t> d2 = utils::parse_number<int32_t>(utils::regex_match_to_view(match[2]));
        if (!d1 || !d2) return u8"你给的数据超出我能处理的范围了……";
        if (*d1 <= 0 || *d2 <= 0) return u8"分布的自由度应该是正整数";
        const float sample = utils::random_fisher_f_float(*d1, *d2);
        return fmt::format(u8"从服从Fisher–Snedecor分布 F({}, {}) 的整体中抽出了样本 {}", *d1, *d2, sample);
    }

    std::optional<std::string> check_student_t(const std::string& dist_str)
    {
        static const boost::regex regex{ fmt::format(R"(t *\( *{} *\))", utils::float_regex) };
        boost::smatch match;
        if (!regex_match(dist_str, match, regex)) return std::nullopt;
        const std::optional<float> nu = utils::parse_number<float>(utils::regex_match_to_view(match[1]));
        if (!nu) return u8"你给的数据超出我能处理的范围了……";
        if (*nu <= 0) return u8"分布的自由度应该是正数";
        const float sample = utils::random_exponential_float(*nu);
        return fmt::format(u8"从服从学生t分布 t({}) 的整体中抽出了样本 {}", *nu, sample);
    }
}

std::string RandomSample::check_distribution(const std::string& dist_str)
{
    if (const std::optional<std::string> result = check_uniform_int(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_uniform_float(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_bernoulli(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_binomial(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_negative_binomial(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_geometric(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_poisson(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_exponential(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_gamma(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_weibull(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_extreme_value(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_normal_float(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_lognormal_float(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_chi_squared(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_cauchy(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_fisher_f(dist_str); result) return *result;
    if (const std::optional<std::string> result = check_student_t(dist_str); result) return *result;
    return u8"我不太懂你说的是什么分布……";
}

bool RandomSample::on_group_msg(const int64_t group, const int64_t user, const std::string& msg)
{
    static const boost::regex regex{ fmt::format(u8R"({}随机(?:抽|采)样 *(.*) *)", utils::at_self_regex) };
    boost::smatch match;
    if (!regex_match(msg, match, regex)) return false;
    utils::reply_group_member(group, user, check_distribution(match[1].str()));
    return true;
}

bool RandomSample::on_private_msg(const int64_t user, const std::string& msg)
{
    static const boost::regex regex{ u8R"( *随机(?:抽|采)样 *(.*) *)" };
    boost::smatch match;
    if (!regex_match(msg, match, regex)) return false;
    cqc::api::send_private_msg(user, check_distribution(match[1].str()));
    return true;
}
