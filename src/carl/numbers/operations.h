/** 
 * @file:   numbers/operations.h
 * @author: Sebastian Junges
 *
 * @since November 6, 2014
 */

#pragma once

#include "../util/SFINAE.h"
#include "constants.h"

namespace carl
{
	template<typename T, EnableIf<has_isZero<T>>>
	bool isZero(const T& t) 
	{
		return t.isZero();
	}
	
	template<typename T, DisableIf<has_isZero<T>>>
	bool isZero(const T& t)
	{
		return t == 0;
	}
	
	template<typename T, EnableIf<has_isOne<T>>>
	bool isOne(const T& t) 
	{
		return t.isOne();
	}
	
	template<typename T, DisableIf<has_isOne<T>>>
	bool isOne(const T& t) 
	{
		return t.isOne();
	}
	
	/**
	 * Implements a fast exponentiation on an arbitrary type T.
	 * To use carl::pow() on a type T, the following must be defined:
	 * - carl::constant_one<T>,
	 * - T::operator=(const T&) and
	 * - operator*(const T&, const T&).
	 * Alternatively, carl::pow() can be specialized for T explicitly.
	 * @param t A number.
	 * @param exp The exponent.
	 * @return `t` to the power of `exp`.
	 */
	template<typename T>
	T pow(const T& t, std::size_t exp) {
		T res = carl::constant_one<T>().get();
		T mult = t;
		for (std::size_t e = exp; e > 0; e /= 2) {
			if (e & 1) res *= mult;
			mult *= mult;
		}
		return res;
	}

	/**
	 * Implements a fast exponentiation on an arbitrary type T.
	 * The result is stored in the given number.
	 * To use carl::pow_assign() on a type T, the following must be defined:
	 * - carl::constant_one<T>,
	 * - T::operator=(const T&) and
	 * - operator*(const T&, const T&).
	 * Alternatively, carl::pow() can be specialized for T explicitly.
	 * @param t A number.
	 * @param exp The exponent.
	 */
	template<typename T>
	void pow_assign(T& t, std::size_t exp) {
		T mult = t;
		t = carl::constant_one<T>().get();
		for (std::size_t e = exp; e > 0; e /= 2) {
			if (e & 1) t *= mult;
			mult *= mult;
		}
	}
	
}