#include "../numbers.h"

#include "parser.h"

#include <limits>

#include <boost/numeric/interval.hpp>

#ifdef CARL_USE_CLN_NUMBERS
namespace carl {
template<>
cln::cl_RA rationalize<cln::cl_RA>(double n) {
    switch (std::fpclassify(n)) {
        case FP_NORMAL:  // normalized are fully supported
            return cln::rationalize(convert<mpq_class, cln::cl_RA>(n));
        case FP_SUBNORMAL: {  // subnormals result in underflows, hence the value of the double is 0.f, where f is the significand precision
            static_assert(sizeof(n) == 8, "double is assumed to be eight bytes wide.");
            auto significandBits = reinterpret_cast<sint>(&n);
            significandBits = (significandBits << 12) >> 12;
            if (n < 0)
                significandBits = -significandBits;
            return cln::cl_RA(significandBits) * ONE_DIVIDED_BY_10_TO_THE_POWER_OF_52;
        }
        case FP_ZERO:
            return 0;
        case FP_NAN:  // NaN and infinite are not supported
        case FP_INFINITE:
            assert(false);
            break;
    }
    return 0;
}

template<>
cln::cl_RA rationalize<cln::cl_RA>(float n) {
    switch (std::fpclassify(n)) {
        case FP_NORMAL:  // normalized are fully supported
            return cln::rationalize(convert<mpq_class, cln::cl_RA>(n));
        case FP_SUBNORMAL: {  // subnormals result in underflows, hence the value of the double is 0.f, where f is the significand precision
            static_assert(sizeof(n) == 4, "float is assumed to be four bytes wide.");
            auto significandBits = reinterpret_cast<sint>(&n);
            significandBits = (significandBits << 9) >> 9;
            if (n < 0)
                significandBits = -significandBits;
            return cln::cl_RA(significandBits) * ONE_DIVIDED_BY_10_TO_THE_POWER_OF_23;
        }
        case FP_ZERO:
            return 0;
        case FP_NAN:  // NaN and infinite are not supported
        case FP_INFINITE:
            assert(false);
            break;
    }
    return 0;
}

bool sqrt_exact(const cln::cl_RA& a, cln::cl_RA& b) {
    if (a < 0)
        return false;
    return cln::sqrtp(a, &b);
}

cln::cl_RA sqrt(const cln::cl_RA& a) {
    auto r = sqrt_safe(a);
    return (r.first + r.second) / 2;
}

cln::cl_RA scaleByPowerOfTwo(const cln::cl_RA& a, int exp) {
    if (exp > 0) {
        return cln::cl_RA(cln::numerator(a) << exp) / cln::denominator(a);
    } else if (exp < 0) {
        return cln::cl_RA(cln::numerator(a)) / (cln::denominator(a) << -exp);
    }
    return a;
}

std::pair<cln::cl_RA, cln::cl_RA> sqrt_safe(const cln::cl_I& num, const cln::cl_I& den) {
    // Compute square roots for numerator and denominator individually
    cln::cl_I root_num;
    cln::cl_I root_den;
    bool num_perfect_square = cln::isqrt(num, &root_num);
    bool den_perfect_square = cln::isqrt(den, &root_den);

    cln::cl_I lower = root_num;
    if (den_perfect_square)
        lower /= root_den;
    else
        lower /= root_den + 1;

    cln::cl_I upper;
    if (num_perfect_square)
        upper = root_num;
    else
        upper = root_num + 1;
    upper /= root_den;

    return std::make_pair(lower, upper);
}

std::pair<cln::cl_RA, cln::cl_RA> sqrt_safe(const cln::cl_RA& a) {
    assert(a >= 0);
    cln::cl_RA exact_root;
    if (cln::sqrtp(a, &exact_root)) {
        // root can be computed exactly.
        return std::make_pair(exact_root, exact_root);
    } else {
        return sqrt_safe(cln::numerator(a), cln::denominator(a));
    }
}

std::pair<cln::cl_RA, cln::cl_RA> sqrt_precision(const cln::cl_RA& a, const cln::cl_RA& prec) {
    assert(a >= 0);
    assert(prec > 0);
    assert(prec < 1);

    // Start with sqrt_safe
    auto res = sqrt_safe(a);
    if (res.second - res.first <= prec * res.second) {
        // Precision was achieved
        return res;
    }

    // Precision was not achieved
    // -> Manually scale numerator and denominator
    // see doc/sqrt_proof.md for details on the scaling factor.
    cln::cl_I num = cln::numerator(a);
    cln::cl_I den = cln::denominator(a);
    assert(num >= 1);
    assert(den >= 1);
    // Scale with 2/precision^2 (1/numerator + 1/denominator)
    cln::cl_RA scale = cln::cl_RA(2) * (cln::cl_RA(1) / num + cln::cl_RA(1) / den) / pow(prec, 2);
    assert(ceil(scale) > 1);
    // Compute sqrt once again but with scaled numbers
    res = sqrt_safe(num * ceil(scale), den * ceil(scale));
    assert(res.second - res.first <= prec * res.second);
    return res;
}

std::pair<cln::cl_RA, cln::cl_RA> sqrt_fast(const cln::cl_RA& a) {
    assert(a >= 0);
    cln::cl_RA exact_root;
    if (cln::sqrtp(a, &exact_root)) {
        // root can be computed exactly.
        return std::make_pair(exact_root, exact_root);
    } else {
        // compute an approximation with sqrt(). we can assume that the surrounding integers contain the actual root.
        // auto factor = cln::integer_length(cln::denominator(a)) - cln::integer_length(cln::numerator(a));
        // if (cln::oddp(factor)) factor += 1;
        cln::cl_I lower = cln::floor1(cln::sqrt(toLF(a)));
        cln::cl_I upper = lower + 1;
        assert(cln::expt_pos(lower, 2) < a);
        assert(cln::expt_pos(upper, 2) > a);
        return std::make_pair(lower, upper);
    }
}

std::pair<cln::cl_RA, cln::cl_RA> root_safe(const cln::cl_RA& a, uint n) {
    auto res = cln::realpart(cln::expt(a, cln::cl_RA(1) / n));
    return std::make_pair(cln::cl_RA(cln::floor1(res)), cln::cl_RA(cln::ceiling1(res)));
}

template<>
cln::cl_RA rationalize<cln::cl_RA>(const std::string& n) {
    return parse<cln::cl_RA>(n);
}

template<>
cln::cl_I parse<cln::cl_I>(const std::string& n) {
    cln::cl_I res;
    bool success = parser::parseDecimal(n, res);
    assert(success);
    return res;
}

template<>
bool try_parse<cln::cl_I>(const std::string& n, cln::cl_I& res) {
    return parser::parseDecimal(n, res);
}

template<>
cln::cl_RA parse<cln::cl_RA>(const std::string& n) {
    cln::cl_RA res;
    bool success = parser::parseRational(n, res);
    assert(success);
    return res;
}

template<>
bool try_parse<cln::cl_RA>(const std::string& n, cln::cl_RA& res) {
    return parser::parseRational(n, res);
}

std::string toString(const cln::cl_RA& _number, bool _infix) {
    std::stringstream s;
    bool negative = (_number < cln::cl_RA(0));
    if (negative)
        s << "(-" << (_infix ? "" : " ");
    if (_infix)
        s << carl::abs(_number);
    else {
        cln::cl_I d = carl::getDenom(_number);
        if (d != cln::cl_I(1))
            s << "(/ " << carl::abs(carl::getNum(_number)) << " " << carl::abs(d) << ")";
        else
            s << carl::abs(_number);
    }
    if (negative)
        s << ")";
    return s.str();
}

std::string toString(const cln::cl_I& _number, bool _infix) {
    std::stringstream s;
    bool negative = (_number < cln::cl_I(0));
    if (negative)
        s << "(-" << (_infix ? "" : " ");
    s << carl::abs(_number);
    if (negative)
        s << ")";
    return s.str();
}
}  // namespace carl
#endif
