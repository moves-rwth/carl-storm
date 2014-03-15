#include "gtest/gtest.h"
#include "carl/core/Variable.h"
#include "carl/core/Monomial.h"
#include "carl/core/Term.h"

using namespace carl;

TEST(Term, Constructor)
{
    Term<mpz_class> t(1);
    Variable v0(1);
    Monomial m0(v0);
    Term<mpz_class> t0(m0);
}

TEST(Term, Operators)
{
    Term<mpz_class> t(1);
    Variable v0(1);
    Monomial m0(v0);
    Term<mpz_class> t0(m0);
    EXPECT_TRUE(t0.isLinear());
    EXPECT_TRUE(t.isLinear());
    EXPECT_EQ((unsigned)0, t.getNrVariables());
}

TEST(Term, Multiplication)
{
    Term<mpz_class> t(1);
    Variable v0(1);
    t *= v0;
    Term<mpz_class> t0(v0);
    EXPECT_EQ(t0,t);
    t *= v0;
    Term<mpz_class> t1(Monomial(v0,2));
    EXPECT_EQ(t1,t);
    
}

TEST(Term, Derivative)
{
    Variable v0(1);
    Variable v1(2);
    Term<mpz_class> t(3);
    t *= v0 * v0 * v0 * v1;
    Term<mpz_class>* tprime = t.derivative(v0);
    EXPECT_EQ(9,tprime->coeff());
}

TEST(Term, Substitute)
{
    Variable v0(1);
    Variable v1(2);
    Term<mpz_class> t(3);
    Term<mpz_class> c4(4);
    t *= v0 * v0 * v1;
    std::map<Variable, Term<mpz_class>> substitutes;
    substitutes.emplace(v1, c4);
    Term<mpz_class>* res = t.substitute(substitutes);
    EXPECT_EQ(12, res->coeff());
    EXPECT_EQ((unsigned)2, res->tdeg());
}
