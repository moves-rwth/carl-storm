#include "EigenWrapper.h"

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

#include <vector>
#include "../../numbers/numbers.h"

namespace carl {
namespace rootfinder {
namespace eigen {

std::vector<double> root_approximation(const std::vector<double>& coeffs) {
	
	using Index = Eigen::MatrixXd::Index;
	// Create companion matrix
	uint degree = coeffs.size();
	Eigen::MatrixXd m = Eigen::MatrixXd::Zero(Index(degree), Index(degree));
	m(0, Index(degree)-1) = -coeffs[0] / coeffs[degree];
	for (uint i = 1; i < degree; ++i) {
		m(Index(i), Index(i)-1) = 1;
		m(Index(i), Index(degree)-1) = -coeffs[i] / coeffs[degree];
	}
	
	// Obtain eigenvalues
	Eigen::VectorXcd eigenvalues = m.eigenvalues();
	
	// Save real parts to tmp
	std::vector<double> tmp(std::size_t(eigenvalues.size()));
	for (uint i = 0; i < std::size_t(eigenvalues.size()); ++i) {
		if (eigenvalues[Index(i)].imag() > eigenvalues[Index(i)].real() / 4) tmp[i] = 0;
		else tmp[i] = eigenvalues[Index(i)].real();
	}
	return tmp;
}

}
}
}
