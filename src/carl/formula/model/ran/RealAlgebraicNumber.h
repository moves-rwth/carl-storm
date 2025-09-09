#pragma once

/**
 * @file
 * Represent a real algebraic number (RAN) in one of several ways:
 * - Implicitly by a univariate polynomial and an interval.
 * - Implicitly by a polynomial and a sequence of signs (called Thom encoding).
 * - Explicitly by a rational number.
 * Rationale:
 * - A real number cannot always be adequately represented in finite memory, since
 *   it may be infinitely long. Representing
 *   it by a float or any other finite-precision representation and doing
 *   arithmatic may introduce unacceptable rouding errors.
 *   The algebraic reals, a subset of the reals, is the set of those reals that can be represented
 *   as the root of a univariate polynomial with rational coefficients so there is always
 *   an implicit, finite, full-precision
 *   representation by an univariate polynomial and an isolating interval that
 *   contains this root (only this root and no other). It is also possible
 *   to do relatively fast arithmetic with this representation without rounding errors.
 * - When the algebraic real is only finitely long prefer the rational number
 *   representation because it's faster.
 * - The idea of the Thom-Encoding is as follows: Take a square-free univariate polynomial p
 *   with degree n that has the algebraic real as its root, compute the first n-1 derivates of p,
 *   plug in this algebraic real into each derivate and only keep the sign.
 *   Then polynomial p with this sequence of signs uniquely represents this algebraic real.
 */

#include <iostream>
#include <memory>

#include "../../../core/MultivariateGCD.h"
#include "../../../core/MultivariatePolynomial.h"
#include "../../../core/Sign.h"
#include "../../../core/UnivariatePolynomial.h"
#include "../../../interval/Interval.h"

#include "RealAlgebraicNumberSettings.h"

#include "../../../thom/ThomEncoding.h"
#include "RealAlgebraicNumber_Interval.h"

namespace carl {

enum class RANSampleHeuristic { Center, CenterInt, LeftInt, RightInt, ZeroInt, InftyInt, Default = RightInt };

template<typename Number>
class RealAlgebraicNumber {
   private:
    template<typename Num>
    friend RealAlgebraicNumber<Num> sampleBelow(const RealAlgebraicNumber<Num>&);
    template<typename Num>
    friend RealAlgebraicNumber<Num> sampleBetween(const RealAlgebraicNumber<Num>&, const RealAlgebraicNumber<Num>&, RANSampleHeuristic heuristic);
    template<typename Num>
    friend RealAlgebraicNumber<Num> sampleAbove(const RealAlgebraicNumber<Num>&);

    using IntervalContent = ran::IntervalContent<Number>;
    using Polynomial = typename IntervalContent::Polynomial;

    mutable Number mValue = carl::constant_zero<Number>::get();
    // A flag/tag that a user of this class can set.
    // It indicates that this number stems from an outside root computation.
    bool mIsRoot = true;
    // Interval representation
    mutable std::shared_ptr<IntervalContent> mIR;
    // ThomEncoding
    std::shared_ptr<ThomEncoding<Number>> mTE;

    void checkForSimplification() const {
        // make numeric if possible
        if (mIR && mIR->interval.isPointInterval()) {
            switchToNR(mIR->interval.lower());
        }
    }
    // Switch to numeric representation.
    void switchToNR(const Number& n) const {
        mValue = n;
        if (mIR) {
            mIR->interval = Interval<Number>(n);
            mIR.reset();
        }
    }

   public:
    RealAlgebraicNumber() = default;
    explicit RealAlgebraicNumber(const Number& n, bool isRoot = true) : mValue(n), mIsRoot(isRoot) {}
    explicit RealAlgebraicNumber(Variable var, bool isRoot = true)
        : mIsRoot(isRoot), mIR(std::make_shared<IntervalContent>(Polynomial(var), Interval<Number>::zeroInterval())) {}
    explicit RealAlgebraicNumber(const Polynomial& p, const Interval<Number>& i, bool isRoot = true)
        : mIsRoot(isRoot), mIR(std::make_shared<IntervalContent>(p.normalized(), i)) {
        assert(!mIR->polynomial.isZero() && mIR->polynomial.degree() > 0);
        assert(i.isOpenInterval() || i.isPointInterval());
        assert(p.countRealRoots(i) == 1);
        if (mIR->polynomial.degree() == 1) {
            Number a = mIR->polynomial.coefficients()[1];
            Number b = mIR->polynomial.coefficients()[0];
            switchToNR(-b / a);
        } else {
            if (i.contains(0))
                refineAvoiding(0);
            refineToIntegrality();
        }
    }
    explicit RealAlgebraicNumber(const Polynomial& p, const Interval<Number>& i, const std::list<UnivariatePolynomial<Number>>& sturmSequence,
                                 bool isRoot = true)
        : mIsRoot(isRoot), mIR(std::make_shared<IntervalContent>(p.normalized(), i, sturmSequence)) {
        assert(!mIR->polynomial.isZero() && mIR->polynomial.degree() > 0);
        assert(i.isOpenInterval() || i.isPointInterval());
        assert(p.countRealRoots(i) == 1);
        if (mIR->polynomial.degree() == 1) {
            Number a = mIR->polynomial.coefficients()[1];
            Number b = mIR->polynomial.coefficients()[0];
            switchToNR(-b / a);
        } else {
            if (i.contains(0))
                refineAvoiding(0);
            refineToIntegrality();
        }
    }

    explicit RealAlgebraicNumber(const ThomEncoding<Number>& te, bool isRoot = true) : mIsRoot(isRoot), mTE(std::make_shared<ThomEncoding<Number>>(te)) {}

    RealAlgebraicNumber(const RealAlgebraicNumber& ran) = default;
    RealAlgebraicNumber(RealAlgebraicNumber&& ran) = default;

    RealAlgebraicNumber& operator=(const RealAlgebraicNumber& n) = default;
    RealAlgebraicNumber& operator=(RealAlgebraicNumber&& n) = default;

    /**
     * Return the size of this representation in memory in number of bits.
     */
    std::size_t size() const {
        if (isNumeric())
            return carl::bitsize(mValue);
        else if (isInterval())
            return carl::bitsize(mIR->interval.lower()) + carl::bitsize(mIR->interval.upper()) * mIR->polynomial.degree();
        else
            return 0;
    }

    /**
     * Check if this number stems from an outside root computation.
     */
    bool isRoot() const {
        return mIsRoot;
    }

    /**
     * Set the flag marking whether the number stems from an outside root computation.
     */
    void setIsRoot(bool isRoot) noexcept {
        mIsRoot = isRoot;
    }

    bool isZero() const {
        if (isNumeric())
            return carl::isZero(mValue);
        else if (isInterval())
            return mIR->interval.isZero();
        else if (isThom())
            return mTE->isZero();
        else
            return false;
    }

    /**
     * Check if the underlying representation is an explicit number.
     */
    bool isNumeric() const {
        checkForSimplification();
        return !mIR && !mTE;
    }
    /**
     * Check if the underlying representation is an implict number
     * (encoded by a polynomial and an interval).
     */
    bool isInterval() const {
        checkForSimplification();
        return bool(mIR);
    }

    /**
     * Check if the underlying representation is an implicit number
     * that uses the Thom encoding.
     */
    bool isThom() const noexcept {
        return bool(mTE);
    }
    const ThomEncoding<Number>& getThomEncoding() const {
        assert(isThom());
        return *mTE;
    }

    bool isIntegral() const {
        refineToIntegrality();
        if (isNumeric())
            return carl::isInteger(mValue);
        else if (isInterval())
            return mIR->isIntegral();
        else
            return false;
    }

    Number integerBelow() const {
        refineToIntegrality();
        if (isNumeric())
            return carl::floor(mValue);
        else if (isInterval())
            return carl::floor(mIR->interval.lower());
        return carl::constant_zero<Number>::get();
    }

    Number branchingPoint() const {
        if (isNumeric())
            return mValue;
        assert(isInterval());
        return mIR->interval.sample();
    }

    const Number& value() const noexcept {
        assert(isNumeric());
        return mValue;
    }

    std::size_t getRefinementCount() const {
        assert(!isNumeric() && !isThom());
        assert(isInterval());
        return mIR->refinementCount;
    }
    const Interval<Number>& getInterval() const {
        assert(isInterval());
        return mIR->interval;
    }
    const Number& lower() const {
        return getInterval().lower();
    }
    const Number& upper() const {
        return getInterval().upper();
    }
    const Polynomial& getIRPolynomial() const {
        assert(!isNumeric());
        assert(isInterval());
        return mIR->polynomial;
    }
    const auto& getIRSturmSequence() const {
        assert(!isNumeric());
        assert(isInterval());
        return mIR->sturmSequence;
    }

    RealAlgebraicNumber changeVariable(Variable v) const {
        if (isNumeric())
            return *this;
        assert(isInterval());
        return RealAlgebraicNumber<Number>(mIR->polynomial.replaceVariable(v), mIR->interval, mIsRoot);
    }

    Sign sgn() const {
        if (isNumeric()) {
            return carl::sgn(mValue);
        } else if (isInterval()) {
            return mIR->interval.sgn();
        } else if (isThom()) {
            return mTE->sgnReprNum();
        } else
            return Sign::ZERO;
    }

    Sign sgn(const Polynomial& p) const {
        if (isNumeric()) {
            return carl::sgn(p.evaluate(mValue));
        } else if (isInterval()) {
            return mIR->sgn(p);
        } else {
            assert(isThom());
            return mTE->signOnPolynomial(MultivariatePolynomial<Number>(p));
        }
    }

    bool isRootOf(const UnivariatePolynomial<Number>& p) const {
        if (isNumeric())
            return p.countRealRoots(value()) == 1;
        else if (isInterval())
            return p.countRealRoots(mIR->interval) == 1;
        else if (isThom())
            return this->sgn(p) == Sign::ZERO;
        else
            return false;
    }

    /**
     * Check if this (possibly implicitly represented) number lies within
     * the bounds of interval 'i'.
     */
    bool containedIn(const Interval<Number>& i) const {
        if (isNumeric())
            return i.contains(mValue);
        else if (isInterval()) {
            if (mIR->interval.contains(i.lower())) {
                refineAvoiding(i.lower());
                if (isNumeric())
                    return i.contains(mValue);
            }
            if (mIR->interval.contains(i.upper())) {
                refineAvoiding(i.upper());
                if (isNumeric())
                    return i.contains(mValue);
            }
            return i.contains(mIR->interval);
        } else if (isThom()) {
            if (i.lowerBoundType() != BoundType::INFTY) {
                if (i.lowerBoundType() == BoundType::STRICT && *mTE <= i.lower())
                    return false;
                if (i.lowerBoundType() == BoundType::WEAK && *mTE < i.lower())
                    return false;
            }
            if (i.upperBoundType() != BoundType::INFTY) {
                if (i.upperBoundType() == BoundType::STRICT && *mTE >= i.upper())
                    return false;
                if (i.upperBoundType() == BoundType::WEAK && *mTE > i.upper())
                    return false;
            }
            return true;
        } else
            return false;
    }

    bool refineAvoiding(const Number& n) const {
        assert(!isNumeric());
        bool res = mIR->refineAvoiding(n);
        checkForSimplification();
        return res;
    }
    /// Refines until the number is either numeric or the interval does not contain any integer.
    void refineToIntegrality() const {
        if (isInterval())
            mIR->refineToIntegrality();
        checkForSimplification();
    }
    void refine() const {
        if (isInterval())
            mIR->refine();
        checkForSimplification();
    }

    void simplifyByPolynomial(Variable var, const MultivariatePolynomial<Number>& poly) const {
        UnivariatePolynomial<Number> irp(var, getIRPolynomial().template convert<Number>().coefficients());
        CARL_LOG_DEBUG("carl.ran", "gcd(" << irp << ", " << poly << ")");
        auto gmv = carl::gcd(MultivariatePolynomial<Number>(irp), poly);
        CARL_LOG_DEBUG("carl.ran", "Simplyfing, gcd = " << gmv);
        if (gmv.isOne())
            return;
        auto g = gmv.toUnivariatePolynomial();
        if (isRootOf(g)) {
            CARL_LOG_DEBUG("carl.ran", "Is a root of " << g);
            mIR->setPolynomial(g);
        } else {
            CARL_LOG_DEBUG("carl.ran", "Is not a root of " << g);
            CARL_LOG_DEBUG("carl.ran", "Dividing " << mIR->polynomial << " by " << g);
            mIR->setPolynomial(mIR->polynomial.divideBy(g.replaceVariable(IntervalContent::auxVariable)).quotient);
        }
    }

    RealAlgebraicNumber<Number> abs() const {
        if (isNumeric())
            return RealAlgebraicNumber<Number>(carl::abs(mValue), mIsRoot);
        if (isInterval()) {
            if (mIR->interval.contains(constant_zero<Number>::get())) {
                mIR->refineAvoiding(constant_zero<Number>::get());
                return abs();
            }
            if (mIR->interval.isPositive())
                return *this;
            return RealAlgebraicNumber<Number>(mIR->polynomial.negateVariable(), mIR->interval.abs(), mIsRoot);
        }
        return RealAlgebraicNumber<Number>();
    }

    bool equal(const RealAlgebraicNumber<Number>& n) const;
    bool less(const RealAlgebraicNumber<Number>& n) const;
    std::pair<bool, bool> checkOrder(const RealAlgebraicNumber<Number>& n) const;

   private:
    bool lessWhileUnequal(const RealAlgebraicNumber<Number>& n) const;
};

template<typename Num>
std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumber<Num>& ran) {
    if (ran.isNumeric())
        return os << "(NR " << ran.value() << (ran.isRoot() ? " R" : "") << ")";
    else if (ran.isInterval())
        return os << "(IR " << ran.getInterval() << ", " << ran.getIRPolynomial() << (ran.isRoot() ? " R" : "") << ")";
    else if (ran.isThom())
        return os << "(TE " << ran.getThomEncoding() << (ran.isRoot() ? " R" : "") << ")";
    else
        return os << "(RAN)";  // should never be the case
}

template<typename Number>
inline bool operator==(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
    return lhs.equal(rhs);
}
template<typename Number>
inline bool operator!=(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
    return !(lhs == rhs);
}
template<typename Number>
inline bool operator<(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
    return lhs.less(rhs);
}
template<typename Number>
inline bool operator<=(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
    return !(rhs < lhs);
}
template<typename Number>
inline bool operator>(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
    return rhs < lhs;
}
template<typename Number>
inline bool operator>=(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
    return rhs <= lhs;
}

}  // namespace carl

namespace std {

template<typename Number>
struct hash<carl::RealAlgebraicNumber<Number>> {
    std::size_t operator()(const carl::RealAlgebraicNumber<Number>& n) const {
        return carl::hash_all(n.isRoot(), n.integerBelow());
    }
};

}  // namespace std

#include "RealAlgebraicNumber.tpp"
#include "RealAlgebraicNumberOperations.h"
