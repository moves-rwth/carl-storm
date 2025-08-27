#include "../Common.h"

#include <carl/core/MultivariatePolynomial.h>
#include <carl/numbers/numbers.h>
#include <carl/util/stringparser.h>

#include <type_traits>
#include <typeinfo>

using namespace carl;

class StringParserTest : public ::testing::Test {
   protected:
    StringParserTest() : spSingleSymbExplicit() {}
    void SetUp() override {
        spSingleSymbExplicit.setImplicitMultiplicationMode(false);
        spSingleSymbExplicit.setVariables({"x", "y", "z"});
    }
    StringParser spSingleSymbExplicit;
};

TEST_F(StringParserTest, termsWithExplicitMultiplication) {
    const StringParser& sp = spSingleSymbExplicit;

    EXPECT_NO_THROW(sp.parseTerm<mpq_class>("x*y^3*z^2"));
    Term<mpq_class> t1 = sp.parseTerm<mpq_class>("x*y^3*z^2");
    EXPECT_EQ((unsigned)6, t1.tdeg());
    EXPECT_EQ((unsigned)3, t1.getNrVariables());
    EXPECT_EQ((mpq_class)1, t1.coeff());
    EXPECT_NO_THROW(sp.parseTerm<mpq_class>("x^23*y^4*z"));
    Term<mpq_class> t2 = sp.parseTerm<mpq_class>("x^23*y^4*z");
    EXPECT_EQ((unsigned)28, t2.tdeg());
    EXPECT_EQ((unsigned)3, t2.getNrVariables());
    EXPECT_EQ((mpq_class)1, t2.coeff());
    EXPECT_NO_THROW(sp.parseTerm<mpq_class>("3*y^4*z"));
    Term<mpq_class> t3 = sp.parseTerm<mpq_class>("3*y^4*z");
    EXPECT_EQ((unsigned)5, t3.tdeg());
    EXPECT_EQ((unsigned)2, t3.getNrVariables());
    EXPECT_EQ((mpq_class)3, t3.coeff());
    EXPECT_THROW(sp.parseTerm<mpq_class>("x^y"), InvalidInputStringException);
    EXPECT_THROW(sp.parseTerm<mpq_class>("3^3"), InvalidInputStringException);
}

TEST_F(StringParserTest, polynomialsWithExplicitMultiplication) {
    const StringParser& sp = spSingleSymbExplicit;

    EXPECT_NO_THROW(sp.parseTerm<mpq_class>("x*y^3*z^2"));
    MultivariatePolynomial<mpq_class> p1 = sp.parseMultivariatePolynomial<mpq_class>("x*y^3*z^2");
    EXPECT_EQ((unsigned)1, p1.nrTerms());
    EXPECT_NO_THROW(sp.parseMultivariatePolynomial<mpq_class>("x^23*y^4*z"));
    MultivariatePolynomial<mpq_class> p2 = sp.parseMultivariatePolynomial<mpq_class>("x^23*y^4*z");
    EXPECT_EQ((unsigned)1, p2.nrTerms());
    EXPECT_NO_THROW(sp.parseMultivariatePolynomial<mpq_class>("x + y^2 + x*y"));
    MultivariatePolynomial<mpq_class> p3 = sp.parseMultivariatePolynomial<mpq_class>("x + y^2 + x*y");
    EXPECT_EQ((unsigned)3, p3.nrTerms());
    EXPECT_THROW(sp.parseMultivariatePolynomial<mpq_class>("x^y"), InvalidInputStringException);
    EXPECT_THROW(sp.parseMultivariatePolynomial<mpq_class>("3^3"), InvalidInputStringException);
}

TEST_F(StringParserTest, rationalFunctionsWithExplicitMultiplication) {}
