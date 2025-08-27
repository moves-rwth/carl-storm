#include "carl/core/Variable.h"
#include "carl/numbers/numbers.h"
#include "carl/util/parser/Parser.h"
#include "carl/util/platform.h"
#include "gtest/gtest.h"

#include "../Common.h"

using namespace carl;

TEST(Parser, Polynomial) {
    carl::parser::Parser<MultivariatePolynomial<Rational>> parser;
    carl::Variable x = freshRealVariable("x");
    carl::Variable y = freshRealVariable("y");
    parser.addVariable(x);
    parser.addVariable(y);

    EXPECT_EQ(Rational(1), parser.polynomial("1"));
    EXPECT_EQ(Rational(2) * x, parser.polynomial("2*x"));
    EXPECT_EQ(x, parser.polynomial("x"));
    EXPECT_EQ(x * y, parser.polynomial("x*y"));
    EXPECT_EQ(x * x, parser.polynomial("x*x"));
    EXPECT_EQ(x * x, parser.polynomial("x^2"));

    //    MultivariatePolynomial<Rational> pol1 = parser.polynomial("(2*x^2)+(3*x)+4");
    //    MultivariatePolynomial<Rational> polCheck = MultivariatePolynomial<Rational>(Rational(2)*x*x + Rational(3)*x + Rational(4));
    //    EXPECT_EQ(polCheck, pol1);
    //    MultivariatePolynomial<Rational> pol2 = parser.polynomial("2*x^2+3*x+4");
    //    EXPECT_EQ(polCheck, pol2);
}

TEST(Parser, RationalFunction) {
    using MP = MultivariatePolynomial<Rational>;
    using RF = RationalFunction<MP>;
    carl::parser::Parser<MP> parser;
    carl::Variable x = freshRealVariable("x");
    parser.addVariable(x);

    EXPECT_EQ(RF(MP(Rational(2) * x)), parser.rationalFunction("2*x"));
    EXPECT_EQ(RF(MP(x * x)), parser.rationalFunction("x^2"));
    EXPECT_EQ(RF(MP(Rational(2) * x), MP(x * x)), parser.rationalFunction("2*x / x^2"));
}

TEST(Parser, Formula) {
    using carl::VariableType;
    using FT = Formula<MultivariatePolynomial<Rational>>;
    carl::parser::Parser<MultivariatePolynomial<Rational>> parser;
    carl::Variable b1 = freshBooleanVariable("O4853");
    carl::Variable b2 = freshBooleanVariable("O3838");
    carl::Variable b3 = freshBooleanVariable("O4848");
    carl::Variable b4 = freshBooleanVariable("O4851");
    carl::Variable b5 = freshBooleanVariable("O4849");
    carl::Variable b6 = freshBooleanVariable("O4850");
    carl::Variable b7 = freshBooleanVariable("O6262");
    carl::Variable b8 = freshBooleanVariable("O6285");
    carl::Variable b9 = freshBooleanVariable("O6217");
    carl::Variable b10 = freshBooleanVariable("O8504");
    carl::Variable b11 = freshBooleanVariable("O8665");
    parser.addVariable(b1);
    parser.addVariable(b2);
    parser.addVariable(b3);
    parser.addVariable(b4);
    parser.addVariable(b5);
    parser.addVariable(b6);
    parser.addVariable(b7);
    parser.addVariable(b8);
    parser.addVariable(b9);
    parser.addVariable(b10);
    parser.addVariable(b11);

    FT result = parser.formula(
        "(O4853 IMPLIES (O3838 AND ((((((((O4848) OR (O4851)) OR (O4849)) OR (O4850)) OR (O6262)) OR (O6285)) OR (O6217)) OR (O8504)) AND (NOT O8665)))");
    std::cout << result << std::endl;
    EXPECT_EQ(FT(IMPLIES, {FT(b1), FT(AND, {FT(b2), FT(OR, {FT(OR, {FT(b3), FT(b4)}), FT(OR, {FT(b5), FT(b6), FT(b7)}), FT(OR, {FT(b8), FT(b9), FT(b10)})}),
                                            FT(NOT, FT(b11))})}),
              result);
}
