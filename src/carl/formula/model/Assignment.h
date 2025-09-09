/**
 * @file Assignment.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-01-14
 * @version 2014-10-27

 */

#pragma once

#include "Model.h"

#include "../Formula.h"
#include "ran/RealAlgebraicNumber.h"
#include "sqrtex/SqrtEx.h"
#include "uninterpreted/SortValue.h"
#include "uninterpreted/UFModel.h"

#include <boost/variant.hpp>

#include <map>

namespace carl {

/**
 * Obtains all assignments which can be transformed to rationals and stores them in the passed map.
 * @param _model The model from which to obtain the rational assignments.
 * @param _rationalAssigns The map to store the rational assignments in.
 * @return true, if the entire model could be transformed to rational assignments. (not possible if, e.g., sqrt is contained)
 */
template<typename Rational, typename Poly>
bool getRationalAssignmentsFromModel(const Model<Rational, Poly>& _model, std::map<Variable, Rational>& _rationalAssigns);

/**
 * @param _assignment The assignment for which to check whether the given formula is satisfied by it.
 * @param _formula The formula to be satisfied.
 * @return 0, if this formula is violated by the given assignment;
 *         1, if this formula is satisfied by the given assignment;
 *         2, otherwise.
 */
template<typename Rational, typename Poly>
unsigned satisfies(const Model<Rational, Poly>& _assignment, const Formula<Poly>& _formula);

template<typename Rational, typename Poly>
bool isPartOf(const std::map<Variable, Rational>& _assignment, const Model<Rational, Poly>& _model);

/**
 * @param _model The assignment for which to check whether the given formula is satisfied by it.
 * @param _assignment The map to store the rational assignments in.
 * @param _formula The formula to be satisfied.
 * @return 0, if this formula is violated by the given assignment;
 *         1, if this formula is satisfied by the given assignment;
 *         2, otherwise.
 */
template<typename Rational, typename Poly>
unsigned satisfies(const Model<Rational, Poly>& _model, const std::map<Variable, Rational>& _assignment, const std::map<BVVariable, BVTerm>& bvAssigns,
                   const Formula<Poly>& _formula);

template<typename Rational, typename Poly>
void getDefaultModel(Model<Rational, Poly>& _defaultModel, const UEquality& _constraint, bool _overwrite = true, size_t _seed = 0);
template<typename Rational, typename Poly>
void getDefaultModel(Model<Rational, Poly>& _defaultModel, const BVTerm& _constraint, bool _overwrite = true, size_t _seed = 0);
template<typename Rational, typename Poly>
void getDefaultModel(Model<Rational, Poly>& _defaultModel, const Constraint<Poly>& _constraint, bool _overwrite = true, size_t _seed = 0);
template<typename Rational, typename Poly>
void getDefaultModel(Model<Rational, Poly>& _defaultModel, const Formula<Poly>& _formula, bool _overwrite = true, size_t _seed = 0);

template<typename Rational, typename Poly>
Formula<Poly> representingFormula(const ModelVariable& mv, const Model<Rational, Poly>& model);
}  // namespace carl

#include "Assignment.tpp"
