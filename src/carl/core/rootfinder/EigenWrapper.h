#pragma once

#include <vector>

namespace carl {
namespace rootfinder {
namespace eigen {

/**
 * Compute approximations of the real roots of the univariate polynomials with the given coefficients.
 * This method internally constructs a companion matrix and computes the eigenvalues.
 */
std::vector<double> root_approximation(const std::vector<double>& coeffs);

}  // namespace eigen
}  // namespace rootfinder
}  // namespace carl
