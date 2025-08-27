#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/PrimitiveEuclideanAlgorithm.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/VariablePool.h"
#include "gtest/gtest.h"

using namespace carl;

TEST(PrimitiveEuclidean, Zx) {
    PrimitiveEuclidean gcd;
    Variable x = carl::freshIntegerVariable("x");
    UnivariatePolynomial<mpz_class> a(x, {-36, 42, -84, 48});
    UnivariatePolynomial<mpz_class> b(x, {-30, 44, -10, -4});
    // UnivariatePolynomial<mpz_class> result = gcd(a.normalized(), b.normalized());
    // std::cout << result << std::endl;
}
