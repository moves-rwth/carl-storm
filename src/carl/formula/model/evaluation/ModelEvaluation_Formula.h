#pragma once

#include "../../Formula.h"
#include "../Model.h"

#include "ModelEvaluation_Constraint.h"
#include "ModelEvaluation_PBConstraint.h"

#include "../../../core/logging.h"

namespace carl {
namespace model {
	
	template<typename Rational, typename Poly>
	void substituteSubformulas(Formula<Poly>& f, const Model<Rational,Poly>& m) {
		CARL_LOG_DEBUG("carl.model.evaluation", "Evaluating " << f << " on " << m);
		Formulas<Poly> res = f.subformulas();
		for (auto& r: res) {
			CARL_LOG_DEBUG("carl.model.evaluation", "Evaluating " << r << " on " << m);
			r = substitute(r, m);
			CARL_LOG_DEBUG("carl.model.evaluation", "Result: " << r);
		}
		f = Formula<Poly>(f.getType(), std::move(res));
		CARL_LOG_DEBUG("carl.model.evaluation", "Result: " << f);
	}

	template<typename Rational, typename Poly>
	void evaluateVarCompare(Formula<Poly>& f, const Model<Rational,Poly>& m) {
		CARL_LOG_DEBUG("carl.model.evaluation", "Evaluating " << f << " on " << m);
		assert(f.getType() == FormulaType::VARCOMPARE);
		const auto& vc = f.variableComparison();
		
		ModelValue<Rational,Poly> cmp = vc.value();
		if (cmp.isSubstitution()) {
			// If assigned directly, the shared_ptr<Substitution> goes out of scope before the result is copied into cmp.
			// Therefore, we start by copying the data and overwriting it afterwards.
			CARL_LOG_DEBUG("carl.model.evaluation", "Evaluating " << cmp.asSubstitution() << " on " << m);
			auto res = cmp.asSubstitution()->evaluate(m);
			if (res.isBool() && !res.asBool()) {
				CARL_LOG_DEBUG("carl.model.evaluation", "MVRoot does not exist, returning false");
				if (vc.negated()) {
					f = Formula<Poly>(FormulaType::TRUE);
				} else {
					f = Formula<Poly>(FormulaType::FALSE);
				}
				return;
			}
			CARL_LOG_DEBUG("carl.model.evaluation", "Evaluated substitution " << cmp << " -> " << res);
			cmp = res;
		}
		if (cmp.isSubstitution()) {
			CARL_LOG_DEBUG("carl.model.evaluation", "MVRoot is still a substitution, cannot evaluate.");
			return;
		}
		assert(cmp.isRational() || cmp.isRAN());
		RealAlgebraicNumber<Rational> val = cmp.isRational() ? RealAlgebraicNumber<Rational>(cmp.asRational()) : cmp.asRAN();
		CARL_LOG_DEBUG("carl.model.evaluation", "rhs is " << val);
		
		auto it = m.find(vc.var());
		if (it == m.end()) {
			CARL_LOG_DEBUG("carl.model.evaluation", "Could not evaluate " << vc << " as " << vc.var() << " is not part of the model");
			return;
		}
		const auto& value = m.evaluated(vc.var());
		assert(value.isRational() || value.isRAN());
		RealAlgebraicNumber<Rational> reference = value.isRational() ? RealAlgebraicNumber<Rational>(value.asRational()) : value.asRAN();
		CARL_LOG_DEBUG("carl.model.evaluation", "Reference value: " << vc.var() << " == " << reference);
		
		f = Formula<Poly>(FormulaType::FALSE);
		CARL_LOG_DEBUG("carl.model.evaluation", "Comparison: " << reference << " " << vc.relation() << " " << val);
		switch (vc.relation()) {
			case Relation::EQ:
				if (reference == val.changeVariable(vc.var())) f = Formula<Poly>(FormulaType::TRUE);
				break;
			case Relation::NEQ:
				if (reference != val.changeVariable(vc.var())) f = Formula<Poly>(FormulaType::TRUE);
				break;
			case Relation::LESS:
				if (reference < val.changeVariable(vc.var())) f = Formula<Poly>(FormulaType::TRUE);
				break;
			case Relation::LEQ:
				if (reference <= val.changeVariable(vc.var())) f = Formula<Poly>(FormulaType::TRUE);
				break;
			case Relation::GREATER:
				if (reference > val.changeVariable(vc.var())) f = Formula<Poly>(FormulaType::TRUE);
				break;
			case Relation::GEQ:
				if (reference >= val.changeVariable(vc.var())) f = Formula<Poly>(FormulaType::TRUE);
				break;
		}
		if (vc.negated()) {
			f = f.negated();
			CARL_LOG_DEBUG("carl.model.evaluation", "Applying negation, result is " << f);
		}
	}
	
	template<typename Rational, typename Poly>
	void evaluateVarAssign(Formula<Poly>& f, const Model<Rational,Poly>& m) {
		assert(f.getType() == FormulaType::VARASSIGN);
		const auto& va = f.variableAssignment();
		auto it = m.find(va.var());
		if (it == m.end()) return;
		const auto& value = m.evaluated(va.var());
		const ModelValue<Rational,Poly>& vavalue = va.value();
		if (value == vavalue) {
			f = Formula<Poly>(FormulaType::TRUE);
		} else {
			f = Formula<Poly>(FormulaType::FALSE);
		}
		if (va.negated()) f = f.negated();
	}
	
	/**
	 * Substitutes all variables from a model within a formula.
	 * May fail to substitute some variables, for example if the values are RANs or SqrtEx.
	 */
	template<typename Rational, typename Poly>
	void substituteIn(Formula<Poly>& f, const Model<Rational,Poly>& m) {
		switch (f.getType()) {
			case FormulaType::ITE: {
				f = Formula<Poly>(FormulaType::ITE, substitute(f.condition(), m), substitute(f.firstCase(), m), substitute(f.secondCase(), m));
				break;
			}
			case FormulaType::EXISTS:
				CARL_LOG_WARN("carl.model.evaluation", "Evaluation of exists not yet implemented.");
				break;
			case FormulaType::FORALL:
				CARL_LOG_WARN("carl.model.evaluation", "Evaluation of forall not yet implemented.");
				break;
			case FormulaType::TRUE: break;
			case FormulaType::FALSE: break;
			case FormulaType::BOOL: {
				auto it = m.find(f.boolean());
				if (it == m.end()) break;
				if (it->second.isBool()) {
					f = Formula<Poly>(it->second.asBool() ? FormulaType::TRUE : FormulaType::FALSE);
				} else {
					CARL_LOG_WARN("carl.model.evaluation", "Could not evaluate " << it->first << " as a boolean, value is " << it->second);
				}
				break;
			}
			case FormulaType::NOT: {
				f = Formula<Poly>(FormulaType::NOT, substitute(f.subformula(), m));
				break;
			}
			case FormulaType::IMPLIES: {
				f = Formula<Poly>(FormulaType::IMPLIES, substitute(f.premise(), m), substitute(f.conclusion(), m));
				break;
			}
			case FormulaType::AND: substituteSubformulas(f, m); break;
			case FormulaType::OR: substituteSubformulas(f, m); break;
			case FormulaType::XOR: substituteSubformulas(f, m); break;
			case FormulaType::IFF: substituteSubformulas(f, m); break;
			case FormulaType::CONSTRAINT: {
				auto res = evaluate(f.constraint(), m);
				if (res.isBool()) {
					if (res.asBool()) f = Formula<Poly>(FormulaType::TRUE);
					else f = Formula<Poly>(FormulaType::FALSE);
				} else {
					assert(res.isSubstitution());
					auto subs = res.asSubstitution();
					auto fsubs = static_cast<ModelFormulaSubstitution<Rational,Poly>*>(subs.get());
					f = fsubs->getFormula();
				}
				break;
			}
			case FormulaType::VARCOMPARE: {
				evaluateVarCompare(f, m);
				break;
			}
			case FormulaType::VARASSIGN: {
				evaluateVarAssign(f, m);
				break;
			}
			case FormulaType::BITVECTOR: {
				BVConstraint bvc = substitute(f.bvConstraint(), m);
				if (bvc.isAlwaysConsistent()) f = Formula<Poly>(FormulaType::TRUE);
				else if (bvc.isAlwaysInconsistent()) f = Formula<Poly>(FormulaType::FALSE);
				else f = Formula<Poly>(bvc);
				break;
			}
			case FormulaType::UEQ: {
				std::set<UVariable> vars;
				f.uequality().collectUVariables(vars);
				if (m.contains(vars)) {
					auto val = evaluate(f.uequality(), m);
					assert(val.isBool());
					f = Formula<Poly>(val.asBool() ? FormulaType::TRUE : FormulaType::FALSE);
				} else {
					CARL_LOG_WARN("carl.model.evaluation", "Could not evaluate " << f.uequality() << " as some variables are missing from the model.");
				}
				break;
			}
			case FormulaType::PBCONSTRAINT: {
				CARL_LOG_ERROR("carl.model.evaluation", "Formula type PBCONSTRAINT is not supported anymore.");
				break;
			}
		}
	}
	
	/**
	 * Evaluates a formula to a ModelValue over a Model.
	 * If evaluation can not be done for some variables, the result may actually be a ModelPolynomialSubstitution.
	 */
	template<typename Rational, typename Poly>
	void evaluate(ModelValue<Rational,Poly>& res, Formula<Poly>& f, const Model<Rational,Poly>& m) {
		substituteIn(f, m);
		if (f.isTrue()) res = true;
		else if (f.isFalse()) res = false;
		else res = createSubstitution<Rational,Poly,ModelFormulaSubstitution<Rational,Poly>>(f);
	}
}
}
