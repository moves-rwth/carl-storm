
/**
 * @file MonomialPool.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#include "Monomial.h"
#include "MonomialPool.h"
#include "logging.h"

namespace carl {
Monomial::~Monomial() {
#ifdef PRUNE_MONOMIAL_POOL
    MonomialPool::getInstance().free(this);
#endif
}
Monomial::Arg Monomial::dropVariable(Variable v) const {
    ///@todo this should work on the shared_ptr directly. Then we could directly return this shared_ptr instead of the ugly copying.
    CARL_LOG_FUNC("carl.core.monomial", mExponents << ", " << v);
    auto it = std::find(mExponents.cbegin(), mExponents.cend(), v);

    if (it == mExponents.cend()) {
        std::vector<std::pair<Variable, exponent>> exps(this->mExponents);
        return MonomialPool::getInstance().create(std::move(exps), mTotalDegree);
    }
    if (mExponents.size() == 1)
        return nullptr;

    uint tDeg = mTotalDegree - it->second;
    Content newExps(mExponents.begin(), it);
    it++;
    newExps.insert(newExps.end(), it, mExponents.end());
    return MonomialPool::getInstance().create(std::move(newExps), tDeg);
}

bool Monomial::divide(Variable v, Monomial::Arg& res) const {
    auto it = std::find(mExponents.cbegin(), mExponents.cend(), v);
    if (it == mExponents.cend())
        return false;
    else {
        Content newExps;
        // If the exponent is one, the variable does not occur in the new monomial.
        if (it->second == 1) {
            if (it != mExponents.begin()) {
                newExps.assign(mExponents.begin(), it);
            }
            newExps.insert(newExps.end(), it + 1, mExponents.end());
        } else {
            // We have to decrease the exponent of the variable by one.
            newExps.assign(mExponents.begin(), mExponents.end());
            newExps[uint(it - mExponents.begin())].second -= 1;
        }
        if (newExps.empty()) {
            res = nullptr;
        } else {
            res = MonomialPool::getInstance().create(std::move(newExps), uint(mTotalDegree - 1));
        }

        return true;
    }
}

bool Monomial::divide(const Monomial::Arg& m, Monomial::Arg& res) const {
    CARL_LOG_FUNC("carl.core.monomial", *this << ", " << m);
    if (!m) {
        res = carl::createMonomial(Content(mExponents), mTotalDegree);
        return true;
    }
    if (m->mTotalDegree > mTotalDegree || m->mExponents.size() > mExponents.size()) {
        // Division will fail.
        CARL_LOG_TRACE("carl.core.monomial", "Result: nullptr");
        return false;
    }
    Content newExps;

    // Linear, as we expect small monomials.
    auto itright = m->mExponents.begin();
    for (auto itleft = mExponents.begin(); itleft != mExponents.end(); ++itleft) {
        // Done with division
        if (itright == m->mExponents.end()) {
            // Insert remaining part
            newExps.insert(newExps.end(), itleft, mExponents.end());
            res = MonomialPool::getInstance().create(std::move(newExps), uint(mTotalDegree - m->mTotalDegree));
            CARL_LOG_TRACE("carl.core.monomial", "Result: " << res);
            return true;
        }
        // Variable is present in both monomials.
        if (itleft->first == itright->first) {
            if (itleft->second < itright->second) {
                // Underflow, itright->exp was larger than itleft->exp.
                CARL_LOG_TRACE("carl.core.monomial", "Result: nullptr");
                return false;
            }
            uint newExp = itleft->second - itright->second;
            if (newExp > 0) {
                newExps.emplace_back(itleft->first, newExp);
            }
            itright++;
        }
        // Variable is not present in lhs, division fails.
        else if (itleft->first > itright->first) {
            CARL_LOG_TRACE("carl.core.monomial", "Result: nullptr");
            return false;
        } else {
            assert(itleft->first < itright->first);
            newExps.emplace_back(*itleft);
        }
    }
    // If there remain variables in the m, it fails.
    if (itright != m->mExponents.end()) {
        CARL_LOG_TRACE("carl.core.monomial", "Result: nullptr");
        return false;
    }
    if (newExps.empty()) {
        CARL_LOG_TRACE("carl.core.monomial", "Result: nullptr");
        res = nullptr;
        return true;
    }
    res = MonomialPool::getInstance().create(std::move(newExps), uint(mTotalDegree - m->mTotalDegree));
    CARL_LOG_TRACE("carl.core.monomial", "Result: " << res);
    return true;
}

Monomial::Arg Monomial::sqrt() const {
    if (mTotalDegree % 2 == 1)
        return nullptr;
    Content newExps;
    for (const auto& it : mExponents) {
        if (it.second % 2 == 1)
            return nullptr;
        newExps.emplace_back(it.first, it.second / 2);
    }
    return createMonomial(std::move(newExps), mTotalDegree / 2);
}

Monomial::Arg Monomial::separablePart() const {
    Content newExps;
    for (auto& it : mExponents) {
        newExps.emplace_back(it.first, 1);
    }
    return MonomialPool::getInstance().create(std::move(newExps), uint(mExponents.size()));
}

Monomial::Arg Monomial::pow(uint exp) const {
    if (exp == 0) {
        return nullptr;
    }
    Content newExps;
    uint expsum = 0;
    for (auto& it : mExponents) {
        newExps.emplace_back(it.first, uint(it.second * exp));
        expsum += newExps.back().second;
    }
    return createMonomial(std::move(newExps), expsum);
}

std::pair<std::size_t, Monomial::Arg> Monomial::derivative(Variable v) const {
    CARL_LOG_FUNC("carl.core.monomial", *this << ", " << v);

    // Linear implementation, as we expect very small monomials.
    auto it = std::find(mExponents.cbegin(), mExponents.cend(), v);
    if (it == mExponents.cend()) {
        return std::make_pair(0, nullptr);
    } else if (it->second == 1) {
        // If the exponent is one, the variable does not occur in the new monomial.
        if (mExponents.size() == 1) {
            // If it was the only variable, we get the one-term.
            return std::make_pair(1, nullptr);
        } else {
            std::vector<std::pair<Variable, exponent>> newExps;
            newExps.assign(mExponents.begin(), it);
            newExps.insert(newExps.end(), it + 1, mExponents.end());
            return std::make_pair(1, createMonomial(std::move(newExps), mTotalDegree - 1));
        }
    } else {
        // We have to decrease the exponent of the variable by one.
        std::vector<std::pair<Variable, exponent>> newExps;
        newExps.assign(mExponents.begin(), mExponents.end());
        newExps[uint(it - mExponents.begin())].second -= exponent(1);
        return std::make_pair(it->second, createMonomial(std::move(newExps), mTotalDegree - 1));
    }
}

std::string Monomial::toString(bool infix, bool friendlyVarNames) const {
    if (mExponents.empty())
        return "1";
    std::stringstream ss;
    if (infix) {
        for (auto vp = mExponents.begin(); vp != mExponents.end(); ++vp) {
            if (vp != mExponents.begin())
                ss << "*";
            ss << VariablePool::getInstance().getName(vp->first, friendlyVarNames);
            if (vp->second > 1)
                ss << "^" << vp->second;
        }
    } else {
        if (mExponents.size() > 1)
            ss << "(* ";
        for (auto vp = mExponents.begin(); vp != mExponents.end(); ++vp) {
            if (vp != mExponents.begin())
                ss << " ";
            if (vp->second == 1)
                ss << VariablePool::getInstance().getName(vp->first, friendlyVarNames);
            else {
                std::string varName = VariablePool::getInstance().getName(vp->first, friendlyVarNames);
                ss << "(*";
                for (uint i = 0; i < vp->second; i++) ss << " " << varName;
                ss << ")";
            }
        }
        if (mExponents.size() > 1)
            ss << ")";
    }
    return ss.str();
}

Monomial::Arg Monomial::gcd(const Monomial::Arg& lhs, const Monomial::Arg& rhs) {
    if (!lhs && !rhs)
        return nullptr;
    if (!lhs)
        return rhs;
    if (!rhs)
        return lhs;

    CARL_LOG_FUNC("carl.core.monomial", lhs << ", " << rhs);
    assert(lhs->isConsistent());
    assert(rhs->isConsistent());

    Content newExps;
    uint expsum = 0;
    // Linear, as we expect small monomials.
    auto itright = rhs->mExponents.cbegin();
    auto leftEnd = lhs->mExponents.cend();
    auto rightEnd = rhs->mExponents.cend();
    for (auto itleft = lhs->mExponents.cbegin(); (itleft != leftEnd && itright != rightEnd);) {
        // Variable is present in both monomials.
        if (itleft->first == itright->first) {
            uint newExp = std::min(itleft->second, itright->second);
            newExps.emplace_back(itleft->first, newExp);
            expsum += newExp;
            ++itright;
            ++itleft;
        } else if (itleft->first < itright->first) {
            ++itleft;
        } else {
            assert(itleft->first > itright->first);
            ++itright;
        }
    }
    // Insert remaining part
    std::shared_ptr<const Monomial> result;
    if (!newExps.empty()) {
        result = createMonomial(std::move(newExps), expsum);
    }
    CARL_LOG_TRACE("carl.core.monomial", "Result: " << result);
    return result;
}

Monomial::Arg Monomial::lcm(const std::shared_ptr<const Monomial>& lhs, const std::shared_ptr<const Monomial>& rhs) {
    if (!lhs && !rhs)
        return nullptr;
    if (!lhs)
        return rhs;
    if (!rhs)
        return lhs;
    CARL_LOG_FUNC("carl.core.monomial", lhs << ", " << rhs);
    assert(lhs->isConsistent());
    assert(rhs->isConsistent());

    Content newExps;
    uint expsum = lhs->tdeg() + rhs->tdeg();
    // Linear, as we expect small monomials.
    auto itright = rhs->mExponents.cbegin();
    auto leftEnd = lhs->mExponents.cend();
    auto rightEnd = rhs->mExponents.cend();
    for (auto itleft = lhs->mExponents.cbegin(); itleft != leftEnd;) {
        // Done on right
        if (itright == rightEnd) {
            // Insert remaining part
            newExps.insert(newExps.end(), itleft, lhs->mExponents.end());
            std::shared_ptr<const Monomial> result = MonomialPool::getInstance().create(std::move(newExps), expsum);
            CARL_LOG_TRACE("carl.core.monomial", "Result: " << result);
            return result;
        }
        // Variable is present in both monomials.
        if (itleft->first == itright->first) {
            uint newExp = std::max(itleft->second, itright->second);
            newExps.emplace_back(itleft->first, newExp);
            expsum -= std::min(itleft->second, itright->second);
            ++itright;
            ++itleft;
        }
        // Variable is not present in lhs, dividing lcm yields variable will not occur in result

        else if (itleft->first > itright->first) {
            newExps.push_back(*itright);
            ++itright;
        } else {
            assert(itleft->first < itright->first);
            newExps.push_back(*itleft);
            ++itleft;
        }
    }
    // Insert remaining part
    newExps.insert(newExps.end(), itright, rhs->mExponents.end());
    std::shared_ptr<const Monomial> result = MonomialPool::getInstance().create(std::move(newExps), expsum);
    CARL_LOG_TRACE("carl.core.monomial", "Result: " << result);
    return result;
}

bool Monomial::isConsistent() const {
    CARL_LOG_FUNC("carl.core.monomial", mExponents << ", " << mTotalDegree << ", " << mHash);
    if (mTotalDegree < 1)
        return false;
    uint tdegree = 0;
    Variable lastVar = Variable::NO_VARIABLE;
    for (const auto& ve : mExponents) {
        if (ve.second <= 0) {
            CARL_LOG_TRACE("carl.core.monomial", "Degree is zero.");
            return false;
        }
        if (lastVar != Variable::NO_VARIABLE) {
            if (ve.first <= lastVar)
                return false;
        }
        tdegree += ve.second;
        lastVar = ve.first;
    }
    if (tdegree != mTotalDegree) {
        CARL_LOG_TRACE("carl.core.monomial", "Wrong total degree.");
        return false;
    }
    if (!std::is_sorted(mExponents.begin(), mExponents.end(),
                        [](const std::pair<Variable, exponent>& p1, const std::pair<Variable, exponent>& p2) { return p1.first < p2.first; })) {
        CARL_LOG_TRACE("carl.core.monomial", "Is not sorted.");
        return false;
    }
    return true;
}

CompareResult Monomial::lexicalCompare(const Monomial& lhs, const Monomial& rhs) {
    assert((&lhs != &rhs) || (lhs.id() == rhs.id()));
    assert((lhs.id() != 0) && (rhs.id() != 0));
    if (lhs.id() == rhs.id())
        return CompareResult::EQUAL;
    auto lhsit = lhs.mExponents.begin();
    auto rhsit = rhs.mExponents.begin();
    auto lhsend = lhs.mExponents.end();
    auto rhsend = rhs.mExponents.end();
    while (lhsit != lhsend) {
        if (rhsit == rhsend)
            return CompareResult::GREATER;
        // which variable occurs first
        if (lhsit->first == rhsit->first) {
            // equal variables
            if (lhsit->second > rhsit->second)
                return CompareResult::LESS;
            if (lhsit->second < rhsit->second)
                return CompareResult::GREATER;
        } else {
            return (lhsit->first < rhsit->first) ? CompareResult::LESS : CompareResult::GREATER;
        }
        ++lhsit;
        ++rhsit;
    }
    assert(rhsit != rhsend);
    return CompareResult::LESS;
}

Monomial::Arg operator*(const Monomial::Arg& lhs, const Monomial::Arg& rhs) {
    CARL_LOG_FUNC("carl.core.monomial", lhs << ", " << rhs);
    if (!lhs)
        return rhs;
    if (!rhs)
        return lhs;
    assert(rhs->tdeg() > 0);
    assert(lhs->tdeg() > 0);
    assert(lhs->isConsistent());
    assert(rhs->isConsistent());
    Monomial::Content newExps;
    newExps.reserve(lhs->exponents().size() + rhs->exponents().size());

    // Linear, as we expect small monomials.
    auto itleft = lhs->begin();
    auto itright = rhs->begin();
    while (itleft != lhs->end() && itright != rhs->end()) {
        // Variable is present in both monomials.
        if (itleft->first == itright->first) {
            newExps.emplace_back(itleft->first, itleft->second + itright->second);
            ++itleft;
            ++itright;
        }
        // Variable is not present in lhs, we have to insert var-exp pair from rhs.
        else if (itleft->first > itright->first) {
            newExps.emplace_back(itright->first, itright->second);
            ++itright;
        }
        // Variable is not present in rhs, we have to insert var-exp pair from lhs.
        else {
            newExps.emplace_back(itleft->first, itleft->second);
            ++itleft;
        }
    }
    // Insert remaining.
    if (itleft != lhs->end())
        newExps.insert(newExps.end(), itleft, lhs->end());
    else if (itright != rhs->end())
        newExps.insert(newExps.end(), itright, rhs->end());
    Monomial::Arg result = createMonomial(std::move(newExps), lhs->tdeg() + rhs->tdeg());
    CARL_LOG_TRACE("carl.core.monomial", "Result: " << result);
    return result;
}

Monomial::Arg operator*(const Monomial::Arg& lhs, Variable rhs) {
    if (!lhs) {
        return MonomialPool::getInstance().create(rhs, 1);
    }
    Monomial::Content newExps;
    // Linear, as we expect small monomials.
    bool inserted = false;
    for (const auto& p : *lhs) {
        if (inserted)
            newExps.push_back(p);
        else if (p.first < rhs)
            newExps.push_back(p);
        else if (p.first == rhs) {
            newExps.emplace_back(rhs, p.second + 1);
            inserted = true;
        } else if (p.first > rhs) {
            newExps.emplace_back(rhs, 1);
            newExps.push_back(p);
            inserted = true;
        }
    }
    if (!inserted)
        newExps.emplace_back(rhs, 1);
    return MonomialPool::getInstance().create(std::move(newExps), lhs->tdeg() + 1);
}

Monomial::Arg operator*(Variable lhs, const Monomial::Arg& rhs) {
    return rhs * lhs;
}

Monomial::Arg operator*(Variable lhs, Variable rhs) {
    Monomial::Content newExps;
    if (lhs < rhs) {
        newExps.emplace_back(lhs, 1);
        newExps.emplace_back(rhs, 1);
    } else if (lhs > rhs) {
        newExps.emplace_back(rhs, 1);
        newExps.emplace_back(lhs, 1);
    } else
        newExps.emplace_back(lhs, 2);
    return MonomialPool::getInstance().create(std::move(newExps), 2);
}

Monomial::Arg pow(Variable v, std::size_t exp) {
    return createMonomial(v, exp);
}
}  // namespace carl
