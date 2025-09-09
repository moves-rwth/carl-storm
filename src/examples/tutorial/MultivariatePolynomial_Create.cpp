#include <iostream>

#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/Variable.h"
#include "carl/numbers/numbers.h"

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif

int main() {
    carl::Variable x = carl::freshRealVariable("x");
    carl::Variable y = carl::freshRealVariable("y");

    /*
     * A carl::MultivariatePolynomial represents a monomial in multiple
     * variables. It consists of a vector of carl::Term objects that consist of
     * a carl::Monomial and a coefficient.
     */

    carl::MultivariatePolynomial<Rational> p(x * y);
    std::cout << p << std::endl;
}
