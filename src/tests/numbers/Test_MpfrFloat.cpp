/**
 * @file Test_MpfrFloat.cpp
 * @author Benedikt Seidl
 */

#include "../../carl/numbers/numbers.h"
#include "gtest/gtest.h"

#ifdef USE_MPFR_FLOAT
TEST(mpfrFloatTest, Constructor) {
    // double constructor tests
    double dVal = 4;
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf1 = carl::FLOAT_T<mpfr_t>(dVal));
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf2 = carl::FLOAT_T<mpfr_t>(dVal, carl::CARL_RND::A));
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf3 = carl::FLOAT_T<mpfr_t>(dVal, carl::CARL_RND::A, 10));
    dVal = 0;
    EXPECT_TRUE(mpfr_zero_p(carl::FLOAT_T<mpfr_t>(dVal).value()) != 0);

    // float constructor tests
    float fVal = 4;
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf1 = carl::FLOAT_T<mpfr_t>(fVal));
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf2 = carl::FLOAT_T<mpfr_t>(fVal, carl::CARL_RND::A));
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf3 = carl::FLOAT_T<mpfr_t>(fVal, carl::CARL_RND::A, 10));
    fVal = 0;
    EXPECT_TRUE(mpfr_zero_p(carl::FLOAT_T<mpfr_t>(fVal).value()) != 0);

    // integer constructor tests
    int iVal = 4;
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf1 = carl::FLOAT_T<mpfr_t>(iVal));
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf2 = carl::FLOAT_T<mpfr_t>(iVal, carl::CARL_RND::A));
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf3 = carl::FLOAT_T<mpfr_t>(iVal, carl::CARL_RND::A, 10));
    iVal = 0;
    EXPECT_TRUE(mpfr_zero_p(carl::FLOAT_T<mpfr_t>(iVal).value()) != 0);

    // mpfr constructor tests
    mpfr_t mVal;
    mpfr_init(mVal);
    mpfr_set_d(mVal, 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899, MPFR_RNDNA);
    carl::FLOAT_T<mpfr_t> hf1 = carl::FLOAT_T<mpfr_t>(mVal);
    // std::cout << hf1 << std::endl;
    hf1.setPrecision(10);
    EXPECT_EQ(10, hf1.precision());
    // std::cout << hf1 << std::endl;

    // copy constructor test
    ASSERT_EQ(hf1, carl::FLOAT_T<mpfr_t>(hf1));

    SUCCEED();
}

TEST(mpfrFloatTest, Hash) {
    std::hash<carl::FLOAT_T<mpfr_t>> hasher;
    hasher(carl::FLOAT_T<mpfr_t>(10));
    hasher(carl::FLOAT_T<mpfr_t>(-12.23648192394727230203828));

    EXPECT_EQ(hasher(carl::FLOAT_T<mpfr_t>(10)), hasher(carl::FLOAT_T<mpfr_t>(10)));
    EXPECT_EQ(hasher(carl::FLOAT_T<mpfr_t>(0)), hasher(carl::FLOAT_T<mpfr_t>(0)));

    SUCCEED();
}

/*
 * Test the following operations
 * 7 > 2
 * 2 < 7
 * 7 != 2
 * 7 >= 7
 * 7 <= 7
 * 7 == 7
 */
TEST(mpfrFloatTest, BooleanOperators) {
    double v1 = 7;
    carl::FLOAT_T<mpfr_t> f1 = carl::FLOAT_T<mpfr_t>(v1);
    double v2 = 2;
    carl::FLOAT_T<mpfr_t> f2 = carl::FLOAT_T<mpfr_t>(v2);
    double v3 = 7;
    carl::FLOAT_T<mpfr_t> f3 = carl::FLOAT_T<mpfr_t>(v3);

    ASSERT_GT(f1, f2);
    ASSERT_LT(f2, f1);
    ASSERT_NE(f1, f2);
    ASSERT_GE(f1, f3);
    ASSERT_LE(f1, f3);
    ASSERT_EQ(f1, f3);
}

/*
 * Test the following operations
 * 7 + 2 = 9
 * 7 + -3 = 4
 * 7 + 0 = 7
 * 7 + -0 = 7
 */
TEST(mpfrFloatTest, Addition) {
    double v1 = 7;
    double v2 = 2;
    double v3 = 7;
    double v4 = -3;
    double v5 = 7;
    double v6 = 0;
    double v7 = 7;
    double v8 = -0;
    double vResult1 = 9;
    double vResult2 = 4;
    double vResult3 = 7;
    double vResult4 = 7;

    carl::FLOAT_T<mpfr_t> f1 = carl::FLOAT_T<mpfr_t>(v1);
    carl::FLOAT_T<mpfr_t> f2 = carl::FLOAT_T<mpfr_t>(v2);
    carl::FLOAT_T<mpfr_t> f3 = carl::FLOAT_T<mpfr_t>(v3);
    carl::FLOAT_T<mpfr_t> f4 = carl::FLOAT_T<mpfr_t>(v4);
    carl::FLOAT_T<mpfr_t> f5 = carl::FLOAT_T<mpfr_t>(v5);
    carl::FLOAT_T<mpfr_t> f6 = carl::FLOAT_T<mpfr_t>(v6);
    carl::FLOAT_T<mpfr_t> f7 = carl::FLOAT_T<mpfr_t>(v7);
    carl::FLOAT_T<mpfr_t> f8 = carl::FLOAT_T<mpfr_t>(v8);
    carl::FLOAT_T<mpfr_t> result1 = carl::FLOAT_T<mpfr_t>(vResult1);
    carl::FLOAT_T<mpfr_t> result2 = carl::FLOAT_T<mpfr_t>(vResult2);
    carl::FLOAT_T<mpfr_t> result3 = carl::FLOAT_T<mpfr_t>(vResult3);
    carl::FLOAT_T<mpfr_t> result4 = carl::FLOAT_T<mpfr_t>(vResult4);

    f1.add_assign(f2, carl::CARL_RND::N);
    ASSERT_EQ(result1, f1);

    f3.add_assign(f4, carl::CARL_RND::N);
    ASSERT_EQ(result2, f3);

    f5.add_assign(f6, carl::CARL_RND::N);
    ASSERT_EQ(result3, f5);

    f7.add_assign(f8, carl::CARL_RND::N);
    ASSERT_EQ(result4, f7);
}

/*
 * Test the following operations
 * 9 - 5 = 4
 * 9 - -4 = 13
 * 9 - 0 = 9
 * 9 - -0 = 9
 */
TEST(mpfrFloatTest, Subtraction) {
    double v1 = 9;
    double v2 = 5;
    double v3 = 9;
    double v4 = -4;
    double v5 = 9;
    double v6 = 0;
    double v7 = 9;
    double v8 = -0;
    double vResult1 = 4;
    double vResult2 = 13;
    double vResult3 = 9;
    double vResult4 = 9;

    double dUnary = 5;
    double dUnaryResult = -5;
    double dUnary2 = -3;
    double dUnaryResult2 = 3;

    carl::FLOAT_T<mpfr_t> f1 = carl::FLOAT_T<mpfr_t>(v1);
    carl::FLOAT_T<mpfr_t> f2 = carl::FLOAT_T<mpfr_t>(v2);
    carl::FLOAT_T<mpfr_t> f3 = carl::FLOAT_T<mpfr_t>(v3);
    carl::FLOAT_T<mpfr_t> f4 = carl::FLOAT_T<mpfr_t>(v4);
    carl::FLOAT_T<mpfr_t> f5 = carl::FLOAT_T<mpfr_t>(v5);
    carl::FLOAT_T<mpfr_t> f6 = carl::FLOAT_T<mpfr_t>(v6);
    carl::FLOAT_T<mpfr_t> f7 = carl::FLOAT_T<mpfr_t>(v7);
    carl::FLOAT_T<mpfr_t> f8 = carl::FLOAT_T<mpfr_t>(v8);
    carl::FLOAT_T<mpfr_t> result1 = carl::FLOAT_T<mpfr_t>(vResult1);
    carl::FLOAT_T<mpfr_t> result2 = carl::FLOAT_T<mpfr_t>(vResult2);
    carl::FLOAT_T<mpfr_t> result3 = carl::FLOAT_T<mpfr_t>(vResult3);
    carl::FLOAT_T<mpfr_t> result4 = carl::FLOAT_T<mpfr_t>(vResult4);

    carl::FLOAT_T<mpfr_t> unary = carl::FLOAT_T<mpfr_t>(dUnary);
    carl::FLOAT_T<mpfr_t> unaryResult = carl::FLOAT_T<mpfr_t>(dUnaryResult);
    carl::FLOAT_T<mpfr_t> unary2 = carl::FLOAT_T<mpfr_t>(dUnary2);
    carl::FLOAT_T<mpfr_t> unaryResult2 = carl::FLOAT_T<mpfr_t>(dUnaryResult2);

    f1.sub_assign(f2, carl::CARL_RND::N);
    ASSERT_EQ(result1, f1);

    f3.sub_assign(f4, carl::CARL_RND::N);
    ASSERT_EQ(result2, f3);

    f5.sub_assign(f6, carl::CARL_RND::N);
    ASSERT_EQ(result3, f5);

    f7.sub_assign(f8, carl::CARL_RND::N);
    ASSERT_EQ(result4, f7);

    unary = -unary;
    unary2 = -unary2;
    ASSERT_EQ(unaryResult, unary);
    ASSERT_EQ(unaryResult2, unary2);
}

/*
 * Test the following operations
 * 4 * 3 = 12
 * 4 * -5 = -20
 * 4 * 0 = 0
 * 4 * -0 = 0
 */
TEST(mpfrFloatTest, Multiplication) {
    double v1 = 4;
    double v2 = 3;
    double v3 = 4;
    double v4 = -5;
    double v5 = 4;
    double v6 = 0;
    double v7 = 4;
    double v8 = -0;
    double vResult1 = 12;
    double vResult2 = -20;
    double vResult3 = 0;
    double vResult4 = 0;

    carl::FLOAT_T<mpfr_t> f1 = carl::FLOAT_T<mpfr_t>(v1);
    carl::FLOAT_T<mpfr_t> f2 = carl::FLOAT_T<mpfr_t>(v2);
    carl::FLOAT_T<mpfr_t> f3 = carl::FLOAT_T<mpfr_t>(v3);
    carl::FLOAT_T<mpfr_t> f4 = carl::FLOAT_T<mpfr_t>(v4);
    carl::FLOAT_T<mpfr_t> f5 = carl::FLOAT_T<mpfr_t>(v5);
    carl::FLOAT_T<mpfr_t> f6 = carl::FLOAT_T<mpfr_t>(v6);
    carl::FLOAT_T<mpfr_t> f7 = carl::FLOAT_T<mpfr_t>(v7);
    carl::FLOAT_T<mpfr_t> f8 = carl::FLOAT_T<mpfr_t>(v8);
    carl::FLOAT_T<mpfr_t> result1 = carl::FLOAT_T<mpfr_t>(vResult1);
    carl::FLOAT_T<mpfr_t> result2 = carl::FLOAT_T<mpfr_t>(vResult2);
    carl::FLOAT_T<mpfr_t> result3 = carl::FLOAT_T<mpfr_t>(vResult3);
    carl::FLOAT_T<mpfr_t> result4 = carl::FLOAT_T<mpfr_t>(vResult4);

    f1.mul_assign(f2, carl::CARL_RND::N);
    ASSERT_EQ(result1, f1);

    f3.mul_assign(f4, carl::CARL_RND::N);
    ASSERT_EQ(result2, f3);

    f5.mul_assign(f6, carl::CARL_RND::N);
    ASSERT_EQ(result3, f5);

    f7.mul_assign(f8, carl::CARL_RND::N);
    ASSERT_EQ(result4, f7);
}

/*
 * Test the following operations
 * 8 / 4 = 2
 * 8 / -2 = -4
 * -8 / -4 = 2
 * 8 / 0 dies
 */
TEST(mpfrFloatTest, Division) {
    double v1 = 8;
    double v2 = 4;
    double v3 = 8;
    double v4 = -2;
    double v5 = -8;
    double v6 = -4;
    double v7 = 8;
    double v8 = 0;
    double vResult1 = 2;
    double vResult2 = -4;
    double vResult3 = 2;

    carl::FLOAT_T<mpfr_t> f1 = carl::FLOAT_T<mpfr_t>(v1);
    carl::FLOAT_T<mpfr_t> f2 = carl::FLOAT_T<mpfr_t>(v2);
    carl::FLOAT_T<mpfr_t> f3 = carl::FLOAT_T<mpfr_t>(v3);
    carl::FLOAT_T<mpfr_t> f4 = carl::FLOAT_T<mpfr_t>(v4);
    carl::FLOAT_T<mpfr_t> f5 = carl::FLOAT_T<mpfr_t>(v5);
    carl::FLOAT_T<mpfr_t> f6 = carl::FLOAT_T<mpfr_t>(v6);
    carl::FLOAT_T<mpfr_t> f7 = carl::FLOAT_T<mpfr_t>(v7);
    carl::FLOAT_T<mpfr_t> f8 = carl::FLOAT_T<mpfr_t>(v8);
    carl::FLOAT_T<mpfr_t> result1 = carl::FLOAT_T<mpfr_t>(vResult1);
    carl::FLOAT_T<mpfr_t> result2 = carl::FLOAT_T<mpfr_t>(vResult2);
    carl::FLOAT_T<mpfr_t> result3 = carl::FLOAT_T<mpfr_t>(vResult3);

    f1.div_assign(f2, carl::CARL_RND::N);
    ASSERT_EQ(result1, f1);

    f3.div_assign(f4, carl::CARL_RND::N);
    ASSERT_EQ(result2, f3);

    f5.div_assign(f6, carl::CARL_RND::N);
    ASSERT_EQ(result3, f5);
#ifndef NDEBUG
    ASSERT_DEATH(f7.div_assign(f8, carl::CARL_RND::N), ".*");
#endif
}

/*
 * Test the following operations
 * sqrt 16 = 4
 * cbrt 27 = 3
 * root 256 8 = 2
 * sqrt -7 throws
 */
TEST(mpfrFloatTest, Roots) {
    double v1 = 16;
    double v2 = 27;
    double v3 = 256;
    double v4 = -7;
    double vResult1 = 4;
    double vResult2 = 3;
    double vResult3 = 2;

    int i1 = 8;
    carl::FLOAT_T<mpfr_t> f1 = carl::FLOAT_T<mpfr_t>(v1);
    carl::FLOAT_T<mpfr_t> f2 = carl::FLOAT_T<mpfr_t>(v2);
    carl::FLOAT_T<mpfr_t> f3 = carl::FLOAT_T<mpfr_t>(v3);
    carl::FLOAT_T<mpfr_t> f4 = carl::FLOAT_T<mpfr_t>(v4);
    carl::FLOAT_T<mpfr_t> result1 = carl::FLOAT_T<mpfr_t>(vResult1);
    carl::FLOAT_T<mpfr_t> result2 = carl::FLOAT_T<mpfr_t>(vResult2);
    carl::FLOAT_T<mpfr_t> result3 = carl::FLOAT_T<mpfr_t>(vResult3);

    f1.sqrt_assign(carl::CARL_RND::N);
    ASSERT_EQ(result1, f1);

    f2.cbrt_assign(carl::CARL_RND::N);
    ASSERT_EQ(result2, f2);

    f3.root_assign(i1, carl::CARL_RND::N);
    ASSERT_EQ(result3, f3);
#ifndef NDEBUG
    ASSERT_DEATH(f4.sqrt_assign(carl::CARL_RND::N), ".*");
#endif
}

TEST(mpfrFloatTest, ConversionOperators) {}

TEST(mpfrFloatTest, Precision) {}
#endif
