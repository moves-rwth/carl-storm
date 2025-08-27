#pragma once

#include "../core/Relation.h"
#include "../core/Variable.h"
#include "VariableComparison.h"
#include "model/ModelValue.h"

namespace carl {
template<typename Poly>
class VariableAssignment {
   private:
    using Base = VariableComparison<Poly>;

   public:
    using Number = typename Base::Number;
    using MR = typename Base::MR;
    using RAN = typename Base::RAN;

   private:
    Base mComparison;

   public:
    VariableAssignment(Variable::Arg v, const RAN& value, bool negated = false) : mComparison(v, value, Relation::EQ, negated) {}
    VariableAssignment(Variable::Arg v, const Number& value, bool negated = false) : mComparison(v, RAN(value), Relation::EQ, negated) {}

    Variable var() const {
        return mComparison.var();
    }
    const RAN& value() const {
        const auto& val = mComparison.value();
        assert(boost::get<RAN>(&val) != nullptr);
        return boost::get<RAN>(val);
    }
    bool negated() const {
        return mComparison.negated();
    }
    VariableAssignment negation() const {
        return VariableAssignment(var(), value(), !negated());
    }
    operator const VariableComparison<Poly>&() const {
        return mComparison;
    }
    void collectVariables(Variables& vars) const {
        mComparison.collectVariables(vars);
    }

    std::string toString(unsigned = 0, bool = false, bool = true) const {
        std::stringstream ss;
        ss << "(" << var() << (negated() ? " -!> " : " -> ") << value() << ")";
        return ss.str();
    }
};

template<typename Poly>
bool operator==(const VariableAssignment<Poly>& lhs, const VariableAssignment<Poly>& rhs) {
    return static_cast<VariableComparison<Poly>>(lhs) == static_cast<VariableComparison<Poly>>(rhs);
}
template<typename Poly>
bool operator<(const VariableAssignment<Poly>& lhs, const VariableAssignment<Poly>& rhs) {
    return static_cast<VariableComparison<Poly>>(lhs) < static_cast<VariableComparison<Poly>>(rhs);
}
template<typename Poly>
std::ostream& operator<<(std::ostream& os, const VariableAssignment<Poly>& va) {
    return os << va.toString();
}
}  // namespace carl

namespace std {
template<typename Pol>
struct hash<carl::VariableAssignment<Pol>> {
    std::size_t operator()(const carl::VariableAssignment<Pol>& va) const {
        return std::hash<carl::VariableComparison<Pol>>()(va);
    }
};
}  // namespace std
