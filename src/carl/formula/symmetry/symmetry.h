#pragma once

namespace carl {
namespace formula {

/**
 * A symmetry \f$\sigma\f$ represents a bijection on a set of variables.
 * For every entry in the vector we have \f$\sigma(e.first) = e.second\f$.
 */
using Symmetry = std::vector<std::pair<Variable, Variable>>;

/**
 * Represents a list of symmetries.
 */
using Symmetries = std::vector<Symmetry>;

}  // namespace formula
}  // namespace carl

#ifdef USE_BLISS

#include "SymmetryBreaker.h"
#include "SymmetryFinder.h"

namespace carl {
namespace formula {

template<typename Poly>
Symmetries findSymmetries(const Formula<Poly>& f) {
    symmetry::GraphBuilder<Poly> g(f);
    return g.symmetries();
}

template<typename Poly>
Formula<Poly> breakSymmetries(const Symmetries& symmetries, bool onlyFirst = true) {
    Formulas<Poly> res;
    for (const auto& s : symmetries) {
        res.emplace_back(symmetry::lexLeaderConstraint<Poly>(s));
        if (onlyFirst)
            return res.back();
    }
    return Formula<Poly>(FormulaType::AND, std::move(res));
}

template<typename Poly>
Formula<Poly> breakSymmetries(const Formula<Poly>& f, bool onlyFirst = true) {
    return breakSymmetries<Poly>(findSymmetries(f), onlyFirst);
}

}  // namespace formula
}  // namespace carl

#else

namespace carl {
namespace formula {

template<typename Poly>
Symmetries findSymmetries(const Formula<Poly>& f) {
    return Symmetries();
}

template<typename Poly>
Formula<Poly> breakSymmetries(const Symmetries& symmetries, bool onlyFirst = true) {
    return Formula<Poly>(FormulaType::TRUE);
}

template<typename Poly>
Formula<Poly> breakSymmetries(const Formula<Poly>& f, bool onlyFirst = true) {
    return Formula<Poly>(FormulaType::TRUE);
}

}  // namespace formula
}  // namespace carl

#endif
