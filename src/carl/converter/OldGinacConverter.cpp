#include "OldGinacConverter.h"

#ifdef USE_GINAC
namespace carl {
template<typename Poly>
bool OldGinacConverter<Poly>::similar(const GiNaC::ex& a, const GiNaC::ex& b) {
    std::lock_guard<std::recursive_mutex> lock(mMutex);
    if (b.is_zero())
        return a.is_zero();
    GiNaC::ex x = a, y = b;
    while ((!GiNaC::is_exactly_a<GiNaC::numeric>(b)) && GiNaC::divide(x, b, x));
    while ((!GiNaC::is_exactly_a<GiNaC::numeric>(a)) && GiNaC::divide(y, a, y));
    while ((x.is_equal(1)) && (y.is_equal(1))) {
        if (x.is_equal(y))
            return true;
        if (x.is_equal(-y))
            return true;
        if (GiNaC::is_exactly_a<GiNaC::numeric>(x) && GiNaC::is_exactly_a<GiNaC::numeric>(y))
            return true;
        GiNaC::ex g = GiNaC::gcd(x, y);
        if (!g.is_equal(1)) {
            assert(GiNaC::divide(x, g, x));
            assert(GiNaC::divide(y, g, y));
            continue;
        } else {
            GiNaC::ex gx = GiNaC::gcd(x, b);
            GiNaC::ex gy = GiNaC::gcd(y, a);
            if ((gx.is_equal(1)) && (gy.is_equal(1)))
                return false;
            assert(GiNaC::divide(x, gx, x));
            assert(GiNaC::divide(y, gy, y));
        }
    }
    return true;
}
}  // namespace carl

#endif
