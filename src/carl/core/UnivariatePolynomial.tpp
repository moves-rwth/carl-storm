/** 
 * @file:   UnivariatePolynomial.tpp
 * @author: Sebastian Junges
 *
 * @since August 26, 2013
 */

#pragma once

#include <algorithm>
#include <iomanip>
#include "../util/SFINAE.h"
#include "../util/platform.h"
#include "logging.h"
#include "Sign.h"
#include "UnivariatePolynomial.h"

namespace carl
{

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar)
: mMainVar(mainVar), mCoefficients()
{
	
}
template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar, const Coeff& c, exponent e) :
mMainVar(mainVar),
mCoefficients(e+1,(Coeff)c-c) // We would like to use 0 here, but Coeff(0) is not always constructable (some methods need more parameter)
{
	if(c != 0)
	{
		mCoefficients[e] = c;
	}
	else
	{
		mCoefficients.clear();
	}
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar, std::initializer_list<Coeff> coefficients)
: mMainVar(mainVar), mCoefficients(coefficients)
{
	
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar, const std::vector<Coeff>& coefficients)
: mMainVar(mainVar), mCoefficients(coefficients)
{
	
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar, std::vector<Coeff>&& coefficients)
: mMainVar(mainVar), mCoefficients(coefficients)
{
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar, const std::map<unsigned, Coeff>& coefficients)
: mMainVar(mainVar)
{
	mCoefficients.reserve(coefficients.rbegin()->first);
	for( const std::pair<unsigned, Coeff>& expAndCoeff : coefficients)
	{
		if(expAndCoeff.first != mCoefficients.size() + 1)
		{
			mCoefficients.resize(expAndCoeff.first, (Coeff)0);
		}
		mCoefficients.push_back(expAndCoeff.second);
	}
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::~UnivariatePolynomial() {
}

template<typename Coeff>
Coeff UnivariatePolynomial<Coeff>::evaluate(const Coeff& value) const 
{
	Coeff result(0);
	Coeff var(1);
	for(const Coeff& coeff : mCoefficients)
	{
		result += (coeff * var);
		var *= value;
	}
    return result;
}

template<typename Coeff>
template<typename C, EnableIf<is_number<C>>>
void UnivariatePolynomial<Coeff>::substituteIn(const Variable& var, const Coeff& value) {
	if (var == this->mainVar()) {
		this->mCoefficients[0] = this->evaluate(value);
		this->mCoefficients.resize(1);
	}
}

template<typename Coeff>
template<typename C, DisableIf<is_number<C>>>
void UnivariatePolynomial<Coeff>::substituteIn(const Variable& var, const Coeff& value) {
	if (var == this->mainVar()) {
		this->mCoefficients[0] = this->evaluate(value);
		this->mCoefficients.resize(1);
	} else {
		for (unsigned i = 0; i < this->mCoefficients.size(); i++) {
			this->mCoefficients[i].substituteIn(var, value);
		}
	}
}

template<typename Coeff>
template<typename C, EnableIf<is_number<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::substitute(const Variable& var, const Coeff& value) const {
	if (var == this->mainVar()) {
		return this->evaluate(value);
	}
	return *this;
}

template<typename Coeff>
template<typename C, DisableIf<is_number<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::substitute(const Variable& var, const Coeff& value) const {
	if (var == this->mainVar()) {
		UnivariatePolynomial<Coeff> res(this->mainVar());
		for (unsigned i = 0; i < this->mCoefficients.size(); i++) {
			res += this->mCoefficients[i].substitute(var, value);
		}
		return res;
	} else {
		std::vector<Coeff> res(this->mCoefficients.size());
		for (unsigned i = 0; i < res.size(); i++) {
			res[i] = this->mCoefficients[i].substitute(var, value);
		}
		UnivariatePolynomial<Coeff> resp(this->mainVar(), res);
		resp.stripLeadingZeroes();
		return resp;
	}
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::derivative(unsigned nth) const
{
	UnivariatePolynomial<Coeff> result(mMainVar);
	result.mCoefficients.reserve(mCoefficients.size()-nth);
	// nth == 1 is most common case and can be implemented more efficient.
	if(nth == 1)
	{
		typename std::vector<Coeff>::const_iterator it = mCoefficients.begin();
		int i = 0;
		for(it += nth; it != mCoefficients.end(); ++it)
		{
			++i;
			result.mCoefficients.push_back(Coeff(i) * *it);
		}
		return result;
	}
	else
	{
		// here we handle nth > 1.
		int c = 1;
		for(unsigned k = 2; k <= nth; ++k)
		{
			c *= k;
		}
		typename std::vector<Coeff>::const_iterator it = mCoefficients.begin();
		unsigned i = nth;
		for(it += nth; it != mCoefficients.end(); ++it)
		{
			result.mCoefficients.push_back(Coeff(c) * *it);
			++i;
			c /= (i - nth);
			c *= i;
		}
		return result;
	}
}

template<typename Coeff>
template<typename C, DisableIf<is_integer<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::reduce(const UnivariatePolynomial& divisor) const
{
	assert(degree() >= divisor.degree());
	assert(!divisor.isZero());
	if(is_field<Coeff>::value && divisor.isConstant())
	{
		return UnivariatePolynomial<Coeff>(mMainVar);
	}
	//std::cout << *this << " / " << divisor << std::endl;
	unsigned degdiff = degree() - divisor.degree();
	Coeff factor = lcoeff()/divisor.lcoeff();
	UnivariatePolynomial<Coeff> result(mMainVar);
	result.mCoefficients.reserve(mCoefficients.size()-1);
	if(degdiff > 0)
	{
		result.mCoefficients.assign(mCoefficients.begin(), mCoefficients.begin() + degdiff);
	}
	
	// By construction, the leading coefficient will be zero.
	for(unsigned i=0; i < mCoefficients.size() - degdiff -1; ++i)
	{
		result.mCoefficients.push_back(mCoefficients[i + degdiff] - factor * divisor.mCoefficients[i]);
	}
	// strip zeros from the end as we might have pushed zeros.
	result.stripLeadingZeroes();
	
	if(result.degree() < divisor.degree())
	{
		return result;
	}
	else 
	{	
		return result.reduce(divisor);
	}
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::prem(const UnivariatePolynomial<Coeff>& divisor) const
{
	LOG_NOTIMPLEMENTED();
}


/**
 * See Algorithm 2.2 in GZL92.
 * @param a
 * @param b
 * @param s
 * @param t
 * @return 
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::extended_gcd(const UnivariatePolynomial& a, const UnivariatePolynomial& b, UnivariatePolynomial& s, UnivariatePolynomial& t)
{
	assert(a.mMainVar == b.mMainVar);
	assert(a.mMainVar == s.mMainVar);
	assert(a.mMainVar == t.mMainVar);
	
	LOGMSG_DEBUG("carl.core", "UnivEEA: a=" << a << ", b=" << b );
	const Variable& x = a.mMainVar;
	UnivariatePolynomial<Coeff> c = a;
	UnivariatePolynomial<Coeff> d = b;
	c.normalizeCoefficients();
	d.normalizeCoefficients();
	c = c.normalized();
	d = d.normalized();
	
	UnivariatePolynomial<Coeff> c1 = a.one();
	UnivariatePolynomial<Coeff> c2(x);
	
	UnivariatePolynomial<Coeff> d1(x);
	UnivariatePolynomial<Coeff> d2 = a.one();
	
	while(!d.isZero())
	{
		DivisionResult<UnivariatePolynomial<Coeff>> divres = c.divide(d);
		assert(divres.remainder == c - divres.quotient * d);
		UnivariatePolynomial r1 = c1 - divres.quotient*d1;
		UnivariatePolynomial r2 = c2 - divres.quotient*d2;
		LOGMSG_TRACE("carl.core", "UnivEEA: q=" << divres.quotient << ", r=" << divres.remainder);
		LOGMSG_TRACE("carl.core", "UnivEEA: r1=" << c1 << "-" << divres.quotient << "*" << d1 << "==" << c1 - divres.quotient * d1 );
		LOGMSG_TRACE("carl.core", "UnivEEA: r2=" << c2 << "-" << divres.quotient << "*" << d2 << "==" << c2 - divres.quotient * d2 );
		c = d;
		c1 = d1;
		c2 = d2;
		d = divres.remainder;
		d1 = r1;
		d2 = r2;
		c.normalizeCoefficients();
		d.normalizeCoefficients();
		
		LOGMSG_TRACE("carl.core", "UnivEEA: c=" << c << ", d=" << d );
		LOGMSG_TRACE("carl.core", "UnivEEA: c1=" << c1 << ", c2=" << c2 );
		LOGMSG_TRACE("carl.core", "UnivEEA: d1=" << d1 << ", d2=" << d2 );
	}
	s = c1 / (a.lcoeff() * c.lcoeff());
	t = c2 / (b.lcoeff() * c.lcoeff());
	c = c.normalized();
	c.normalizeCoefficients();
	s.normalizeCoefficients();
	t.normalizeCoefficients();
	LOGMSG_DEBUG("carl.core", "UnivEEA: g=" << c << ", s=" << s << ", t=" << t );
	LOGMSG_TRACE("carl.core", "UnivEEA: " << c << "==" << s*a + t*b << "==" << s*a << " + " << t*b );
	assert(c == s*a + t*b);
	return c;
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::gcd(const UnivariatePolynomial& a, const UnivariatePolynomial& b)
{
	// We want degree(b) <= degree(a).
	if(a.degree() < b.degree()) return gcd_recursive(b.normalized(),a.normalized()).normalized();
	else return gcd_recursive(a.normalized(),b.normalized()).normalized();
}


template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::gcd_recursive(const UnivariatePolynomial& a, const UnivariatePolynomial& b)
{
	assert(b.degree() <= a.degree());
	std::cout << "a: " << a << std::endl;
	std::cout << "b: " << b << std::endl;
	if(b.isZero()) return a;
//	if(is_field<Coeff>::value)
//	{
//		if(b.isConstant()) return b;
//	}
	else return gcd_recursive(b, a.reduce(b));
}

template<typename Coeff>
template<typename C, EnableIf<is_fraction<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::squareFreePart() const {
	UnivariatePolynomial normalized = this->coprimeCoefficients().template convert<Coeff>();
	return normalized.divide(UnivariatePolynomial::gcd(normalized, normalized.derivative())).quotient;
}

template<typename Coeff>
template<typename C, DisableIf<is_fraction<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::squareFreePart() const {
	LOG_NOTIMPLEMENTED();
	return *this;
}


template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::mod(const Coeff& modulus)
{
	for(Coeff& coeff : mCoefficients)
	{
		coeff = carl::mod(coeff, modulus);
	}
	return *this;
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::mod(const Coeff& modulus) const
{
	UnivariatePolynomial<Coeff> result;
	result.mCoefficients.reserve(mCoefficients.size());
	for(auto coeff : mCoefficients)
	{
		result.mCoefficients.push_back(mod(coeff, modulus));
	}
	result.stripLeadingZeroes();
	return result;
}

template<typename Coeff>
template<typename NewCoeff>
UnivariatePolynomial<NewCoeff> UnivariatePolynomial<Coeff>::convert() const {
	std::vector<NewCoeff> coeffs;
	coeffs.resize(this->mCoefficients.size());
	for (unsigned int i = 0; i < this->mCoefficients.size(); i++) {
		coeffs[i] = (NewCoeff)(this->mCoefficients[i]);
	}
	return UnivariatePolynomial<NewCoeff>(this->mMainVar, coeffs);
}

template<typename Coeff>
template<typename NewCoeff>
UnivariatePolynomial<NewCoeff> UnivariatePolynomial<Coeff>::convert(const std::function<NewCoeff(const Coeff&)>& f) const {
	std::vector<NewCoeff> coeffs;
	coeffs.resize(this->mCoefficients.size());
	for (unsigned int i = 0; i < this->mCoefficients.size(); i++) {
		coeffs[i] = f(this->mCoefficients[i]);
	}
	return UnivariatePolynomial<NewCoeff>(this->mMainVar, coeffs);
}

template<typename Coeff>
Coeff UnivariatePolynomial<Coeff>::cauchyBound() const
{
	// We could also use SFINAE, but this gives clearer error messages.
	// Just in case, if we want to use SFINAE, the right statement would be
	// template<typename t = Coefficient, typename std::enable_if<is_field<t>::value, int>::type = 0>
	static_assert(is_field<Coeff>::value, "Cauchy bounds are only defined for field-coefficients");
	Coeff maxCoeff = abs(mCoefficients.front());
	for(typename std::vector<Coeff>::const_iterator it = ++mCoefficients.begin(); it != --mCoefficients.end(); ++it)
	{
        Coeff absOfCoeff = abs( *it );
		if(absOfCoeff > maxCoeff) 
		{
			maxCoeff = absOfCoeff;
		}
	}
	return 1 + maxCoeff/abs(lcoeff());
}

template<typename Coeff>
template<typename C, EnableIf<is_field<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::normalized() const
{
	if(isZero())
	{
		return *this;
	}
	Coeff tmp(lcoeff());
	return *this/tmp;
}

template<typename Coeff>
template<typename C, DisableIf<is_field<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::normalized() const
{
	// TODO implement
}
	
template<typename Coeff>
Coeff UnivariatePolynomial<Coeff>::coprimeFactor() const
{
	static_assert(is_number<Coeff>::value, "We can only make integer coefficients if we have a number type before.");
	typename std::vector<Coeff>::const_iterator it = mCoefficients.begin();
	typename IntegralT<Coeff>::type num = getNum(*it);
	typename IntegralT<Coeff>::type den = getDenom(*it);
	for(++it; it != mCoefficients.end(); ++it)
	{
		num = carl::gcd(num, getNum(*it));
		den = carl::lcm(den, getDenom(*it));
	}
	return den/num;
}

template<typename Coeff>
template<typename C, EnableIf<is_fraction<C>>>
UnivariatePolynomial<typename IntegralT<Coeff>::type> UnivariatePolynomial<Coeff>::coprimeCoefficients() const
{
	static_assert(is_number<Coeff>::value, "We can only make integer coefficients if we have a number type before.");
	Coeff factor = coprimeFactor();
	// Notice that even if factor is 1, we create a new polynomial
	UnivariatePolynomial<typename IntegralT<Coeff>::type> result(mMainVar);
	result.mCoefficients.reserve(mCoefficients.size());
	for(const Coeff& coeff : mCoefficients)
	{
		assert(getDenom(coeff*factor) == 1);
		result.mCoefficients.push_back(getNum(coeff * factor));
	}
	return result;
}	

template<typename Coeff>
template<typename C, DisableIf<is_integer<C>>>
DivisionResult<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::divide(const UnivariatePolynomial<Coeff>& divisor) const
{
	assert(!divisor.isZero());
	DivisionResult<UnivariatePolynomial<Coeff>> result(UnivariatePolynomial<Coeff>(mMainVar), *this);
	assert(*this == divisor * result.quotient + result.remainder);
	if(divisor.degree() > degree())
	{
		return result;
	}
	result.quotient.mCoefficients.resize(1+mCoefficients.size()-divisor.mCoefficients.size(),(Coeff)0);
	
	do
	{
		Coeff factor = result.remainder.lcoeff()/divisor.lcoeff();
		unsigned degdiff = result.remainder.degree() - divisor.degree();
		result.remainder -= UnivariatePolynomial<Coeff>(mMainVar, factor, degdiff) * divisor;
		result.quotient.mCoefficients[degdiff] += factor;
	}
	while(divisor.degree() <= result.remainder.degree() && !result.remainder.isZero());
	assert(*this == divisor * result.quotient + result.remainder);
	return result;
}

template<typename Coeff>
template<typename C, EnableIf<is_integer<C>>>
DivisionResult<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::divide(const UnivariatePolynomial<Coeff>& divisor) const
{
	assert(!divisor.isZero());
	DivisionResult<UnivariatePolynomial<Coeff>> result(UnivariatePolynomial<Coeff>(mMainVar), *this);
	assert(*this == divisor * result.quotient + result.remainder);

	result.quotient.mCoefficients.resize(1+mCoefficients.size()-divisor.mCoefficients.size(),(Coeff)0);

	unsigned int degdiff = this->degree() - divisor.degree();
	for (unsigned int offset = 0; offset <= degdiff; offset++) {
		Coeff factor = carl::div(result.remainder.mCoefficients[this->degree()-offset], divisor.lcoeff());
		result.remainder -= UnivariatePolynomial<Coeff>(mMainVar, factor, degdiff - offset) * divisor;
		result.quotient.mCoefficients[degdiff-offset] += factor;
	}
	assert(*this == divisor * result.quotient + result.remainder);
	return result;
}

template<typename Coeff>
bool UnivariatePolynomial<Coeff>::divides(const UnivariatePolynomial& dividant) const
{
	return dividant.divide(*this).remainder.isZero();
}

template<typename Coeff>
Coeff UnivariatePolynomial<Coeff>::modifiedCauchyBound() const
{
	// We could also use SFINAE, but this gives clearer error messages.
	// Just in case, if we want to use SFINAE, the right statement would be
	// template<typename t = Coefficient, typename std::enable_if<is_field<t>::value, int>::type = 0>
	static_assert(is_field<Coeff>::value, "Modified Cauchy bounds are only defined for field-coefficients");
	LOG_NOTIMPLEMENTED();
}

template<typename Coeff>
template<typename C, EnableIf<is_fraction<C>>>
typename UnivariatePolynomial<Coeff>::IntNumberType UnivariatePolynomial<Coeff>::maximumNorm() const {
	typename std::vector<C>::const_iterator it = mCoefficients.begin();
	Coeff max = *it;
	IntNumberType num = getNum(*it);
	IntNumberType den = getDenom(*it);
	for (++it; it != mCoefficients.end(); ++it) {
		if (*it > max) max = *it;
		num = carl::gcd(num, getNum(*it));
		den = carl::lcm(den, getDenom(*it));
	}
	assert(getDenom(max*den/num) == 1);
	return getNum(max*den/num);
}

template<typename Coeff>
template<typename C, EnableIf<is_instantiation_of<GFNumber, C>>>
UnivariatePolynomial<typename IntegralT<Coeff>::type> UnivariatePolynomial<Coeff>::toIntegerDomain() const
{
	UnivariatePolynomial<typename IntegralT<Coeff>::type> res(mMainVar);
	res.mCoefficients.reserve(mCoefficients.size());
	for(const Coeff& c : mCoefficients)
	{
		assert(isInteger(c));
		res.mCoefficients.push_back(c.representingInteger());
	}
	res.stripLeadingZeroes();
	return res;
}

template<typename Coeff>
template<typename C, DisableIf<is_instantiation_of<GFNumber, C>>>
UnivariatePolynomial<typename IntegralT<Coeff>::type> UnivariatePolynomial<Coeff>::toIntegerDomain() const
{
	UnivariatePolynomial<typename IntegralT<Coeff>::type> res(mMainVar);
	res.mCoefficients.reserve(mCoefficients.size());
	for(const Coeff& c : mCoefficients)
	{
		assert(isInteger(c));
		res.mCoefficients.push_back((typename IntegralT<Coeff>::type)c);
	}
	res.stripLeadingZeroes();
}

template<typename Coeff>
//template<typename T = Coeff, EnableIf<!std::is_same<IntegralT<Coeff>, bool>::value>>
UnivariatePolynomial<GFNumber<typename IntegralT<Coeff>::type>> UnivariatePolynomial<Coeff>::toFiniteDomain(const GaloisField<typename IntegralT<Coeff>::type>* galoisField) const
{
	UnivariatePolynomial<GFNumber<typename IntegralT<Coeff>::type>> res(mMainVar);
	res.mCoefficients.reserve(mCoefficients.size());
	for(const Coeff& c : mCoefficients)
	{
		assert(isInteger(c));
		res.mCoefficients.push_back(GFNumber<typename IntegralT<Coeff>::type>(c,galoisField));
	}
	res.stripLeadingZeroes();
	return res;
	
}

template<typename Coeff>
template<typename N, EnableIf<is_fraction<N>>>
typename UnivariatePolynomial<Coeff>::NumberType UnivariatePolynomial<Coeff>::numericContent() const
{
	if (this->isZero()) return 0;
	// Obtain main denominator for all coefficients.
	IntNumberType mainDenom = this->mainDenom();
	
	// now, some coefficient times mainDenom is always integral.
	// we convert such a product to an integral data type by getNum()
	assert(getDenom(this->numericContent(0) * mainDenom) == 1);
	IntNumberType res = getNum(this->numericContent(0) * mainDenom);
	for (unsigned i = 1; i < this->mCoefficients.size(); i++) {
		assert(getDenom(this->numericContent(i) * mainDenom) == 1);
		res = carl::gcd(getNum(this->numericContent(i) * mainDenom), res);
	}
	return res / mainDenom;
}

template<typename Coeff>
template<typename C, EnableIf<is_number<C>>>
typename UnivariatePolynomial<Coeff>::IntNumberType UnivariatePolynomial<Coeff>::mainDenom() const
{
	IntNumberType denom = 1;
	for (unsigned int i = 0; i < this->mCoefficients.size(); i++) {
		denom = carl::lcm(denom, getDenom(this->mCoefficients[i]));
	}
	return denom;
}
template<typename Coeff>
template<typename C, DisableIf<is_number<C>>>
typename UnivariatePolynomial<Coeff>::IntNumberType UnivariatePolynomial<Coeff>::mainDenom() const
{
	IntNumberType denom = 1;
	for (unsigned int i = 0; i < this->mCoefficients.size(); i++) {
		denom = carl::lcm(denom, this->mCoefficients[i].mainDenom());
	}
	return denom;
}

template<typename Coeff>
std::map<UnivariatePolynomial<Coeff>, unsigned> UnivariatePolynomial<Coeff>::factorization() const
{
    LOGMSG_TRACE("carl.core", "UnivFactor: " << *this );
    std::map<UnivariatePolynomial<Coeff>, unsigned> result;
    if(isConstant()) // Constant.
    {
        LOGMSG_TRACE("carl.core", "UnivFactor: add the factor (" << *this << ")^" << 1 );
        result.insert(std::pair<UnivariatePolynomial<Coeff>, unsigned>(*this, 1));
        return result;
    }
    // Make the polynomial's coefficients coprime (integral and with gcd 1).
    UnivariatePolynomial<Coeff> remainingPoly(mainVar());
    Coeff factor = coprimeFactor();
    if(factor == 1)
    {
        remainingPoly = *this;
    }
    else
    {
        // Store the rational factor and make the polynomial's coefficients coprime.
        LOGMSG_TRACE("carl.core", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(mainVar(), (Coeff) 1 / factor) << ")^" << 1 );
        result.insert(std::pair<UnivariatePolynomial<Coeff>, unsigned>(UnivariatePolynomial<Coeff>(mainVar(), (Coeff) 1 / factor), 1));
        remainingPoly.mCoefficients.reserve(mCoefficients.size());
        for(const Coeff& coeff : mCoefficients)
        {
            remainingPoly.mCoefficients.push_back(coeff * factor);
        }
    }
    assert(mCoefficients.size() > 1);
    // Exclude the factors  (x-r)^i  with  r rational.
    remainingPoly = excludeLinearFactors<int>(remainingPoly, result, INT_MAX);
    assert(!remainingPoly.isConstant() || remainingPoly.lcoeff() == (Coeff)1);
    if(!remainingPoly.isConstant())
    {
        // Calculate the square free factorization.
        std::map<unsigned, UnivariatePolynomial<Coeff>> sff = remainingPoly.squareFreeFactorization();
//        factor = (Coeff) 1;
        for(auto expFactorPair = sff.begin(); expFactorPair != sff.end(); ++expFactorPair)
        {
//            Coeff cpf = expFactorPair->second.coprimeFactor();
//            if(cpf != (Coeff) 1)
//            {
//                factor *= pow(expFactorPair->second.coprimeFactor(), expFactorPair->first);
//                expFactorPair->second /= cpf;
//            }
            if(!expFactorPair->second.isConstant() || expFactorPair->second.lcoeff() != (Coeff) 1)
            {
                auto retVal = result.insert(std::pair<UnivariatePolynomial<Coeff>, unsigned>(expFactorPair->second, expFactorPair->first));
                LOGMSG_TRACE("carl.core", "UnivFactor: add the factor (" << expFactorPair->second << ")^" << expFactorPair->first );
                if(!retVal.second)
                {
                    retVal.first->second += expFactorPair->first;
                }
            }
        }
//        if(factor != (Coeff) 1)
//        {
//            LOGMSG_TRACE("carl.core", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(mainVar(), {factor}) << ")^" << 1 );
//            // Add the constant factor to the factors.
//            if( result.begin()->first.isConstant() )
//            {
//                factor *= result.begin()->first.lcoeff();
//                result.erase( result.begin() );
//            }
//            result.insert(result.begin(), std::pair<UnivariatePolynomial<Coeff>, unsigned>(UnivariatePolynomial<Coeff>(mainVar(), {factor}), 1));
//        }
    }
    return result;
}

template<typename Coeff>
template<typename Integer>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::excludeLinearFactors(const UnivariatePolynomial<Coeff>& poly, std::map<UnivariatePolynomial<Coeff>, unsigned>& linearFactors, const Integer& maxInt)
{
    LOGMSG_TRACE("carl.core", "UnivELF: " << poly );
    UnivariatePolynomial<Coeff> result(poly.mainVar());
    // Exclude the factor x^i from result.
    auto cf = poly.coefficients().begin();
    if(*cf == 0) // result is of the form a_n * x^n + ... + a_k * x^k (a>k, k>0)
    {
        unsigned k = 0;
        while(*cf == 0)
        {
            assert(cf != poly.coefficients().end());
            ++cf;
            ++k;
        }
        // Take x^k as a factor.
        auto retVal = linearFactors.insert(std::pair<UnivariatePolynomial<Coeff>, unsigned>(UnivariatePolynomial<Coeff>(poly.mainVar(), {(Coeff)0, (Coeff)1}), k));
        LOGMSG_TRACE("carl.core", "UnivELF: add the factor (" << retVal.first->first << ")^" << k );
        if(!retVal.second)
        {
            retVal.first->second += k;
        }
        // Construct the remainder:  result := a_n * x^{n-k} + ... + a_{k-1} * x + a_k
        std::vector<Coeff> cfs;
        cfs.reserve(poly.coefficients().size()-k);
        cfs = std::vector<Coeff>(cf, poly.coefficients().end());
        result = UnivariatePolynomial<Coeff>(poly.mainVar(), std::move(cfs));
        LOGMSG_TRACE("carl.core", "UnivELF: remainder is  " << result );
    }
    else
    {
        result = poly;
    }
    // Check whether the polynomial is already a linear factor.
    if(result.degree() > 1)
    {
        // Exclude the factor (x-r)^i, with r rational and r!=0, from result.
        assert(result.coefficients().size() > 1);
        typename IntegralT<Coeff>::type lc = abs(getNum(result.lcoeff()));
        typename IntegralT<Coeff>::type tc = abs(getNum(result.coefficients().front()));
        if( maxInt != 0 && (tc > maxInt || lc > maxInt) )
        {
            return result;
        }
        Integer lcAsInt = toInt<Integer>(lc);
        Integer tcAsInt = toInt<Integer>(tc);
        Integer halfOfLcAsInt = lcAsInt == 1 ? 1 : lcAsInt/2;
        Integer halfOfTcAsInt = tcAsInt == 1 ? 1 : tcAsInt/2;
        std::vector<std::pair<Integer, Integer>> shiftedTcs;
        bool positive = true;
        bool tcFactorsFound = false;
        std::vector<Integer> tcFactors = std::vector<Integer>(1, 1); // TODO: store the divisors of some numbers during compilation
        auto tcFactor = tcFactors.begin();
        bool lcFactorsFound = false;
        std::vector<Integer> lcFactors = std::vector<Integer>(1, 1); // TODO: store the divisors of some numbers during compilation
        auto lcFactor = lcFactors.begin();
        while(true)
        {
            LOGMSG_TRACE("carl.core", "UnivELF: try rational  " << (positive ? "-" : "") << *tcFactor << "/" << *lcFactor);
            // Check whether the numerator of the rational to consider divides the trailing coefficient of all
            // zero-preserving shifts {result(x+x_0) | for some found x_0 with result(x_0)!=0 and x_0 integer}
            auto shiftedTc = shiftedTcs.begin();
            for(; shiftedTc != shiftedTcs.end(); ++shiftedTc)
            {
                // we need to be careful with overflows in the following lines
                if(maxInt/(*lcFactor) >= shiftedTc->first)
                {
                    Integer divisor = (*lcFactor) * shiftedTc->first;
                    if( divisor != *tcFactor )
                    {
                        if( !(divisor < 0 && *tcFactor < 0 && maxInt + divisor >= -(*tcFactor)) && !(divisor > 0 && *tcFactor > 0 && maxInt - divisor >= *tcFactor ) )
                        {
                            if( divisor > *tcFactor )
                            {
                                divisor = divisor - *tcFactor;
                            }
                            else
                            {
                                divisor = *tcFactor - divisor;
                            }
                            if(carl::mod(shiftedTc->second, divisor) != 0)
                            {
                                break;
                            }
                        }
                    }
                }
            }
            if(shiftedTc == shiftedTcs.end())
            {
                Coeff posRatZero = positive ? (Coeff(*tcFactor) / Coeff(*lcFactor)) : -(Coeff(*tcFactor) / Coeff(*lcFactor));
                LOGMSG_TRACE("carl.core", "UnivELF: consider possible non zero rational factor  " << posRatZero);
                Coeff image = result.syntheticDivision(posRatZero);
                if(image == 0)
                {
                    // Remove all linear factor with the found zero from result.
                    UnivariatePolynomial<Coeff> linearFactor(result.mainVar(), {-posRatZero, (Coeff)1});
                    while(image == 0)
                    {
                        auto retVal = linearFactors.insert(std::pair<UnivariatePolynomial<Coeff>, unsigned>(linearFactor, 1));
                        LOGMSG_TRACE("carl.core", "UnivELF: add the factor (" << linearFactor << ")^" << 1 );
                        if(!retVal.second)
                        {
                            ++retVal.first->second;
                        }
                        // Check whether result is a linear factor now.
                        if(result.degree() <= 1)
                        {
                            goto LinearFactorRemains;
                        }
                        image = result.syntheticDivision(posRatZero);
                    }
                }
                else if(isInteger(posRatZero))
                {
                    // Add a zero-preserving shift.
                    assert(isInteger(image));
                    typename IntegralT<Coeff>::type imageInt = abs(getNum(image));
                    if( imageInt <= maxInt )
                    {
                        LOGMSG_TRACE("carl.core", "UnivELF: new shift with " << getNum(posRatZero) << " to " << abs(getNum(image)));
                        shiftedTcs.push_back(std::pair<Integer, Integer>(toInt<Integer>(getNum(posRatZero)), toInt<Integer>(abs(getNum(image)))));
                    }
                }
            }
            // Find the next numerator-denominator combination.
            if(shiftedTc == shiftedTcs.end() && positive)
            {
                positive = false;
            }
            else
            {
                positive = true;
                if(lcFactorsFound)
                {
                    ++lcFactor;
                }
                else
                {
                    lcFactors.push_back(lcFactors.back());
                    while(lcFactors.back() <= halfOfLcAsInt)
                    {
                        ++lcFactors.back();
                        if(carl::mod(lcAsInt, lcFactors.back()) == 0)
                        {
                            break;
                        }
                    }
                    if(lcFactors.back() > halfOfLcAsInt)
                    {
                        lcFactors.pop_back();
                        lcFactorsFound = true;
                        lcFactor = lcFactors.end();
                    }
                    else
                    {
                        lcFactor = --(lcFactors.end());
                    }
                }
                if(lcFactor == lcFactors.end())
                {
                    if(tcFactorsFound)
                    {
                        ++tcFactor;
                    }
                    else
                    {
                        tcFactors.push_back(tcFactors.back());
                        while(tcFactors.back() <= halfOfTcAsInt)
                        {
                            ++(tcFactors.back());
                            if(carl::mod(tcAsInt, tcFactors.back()) == 0)
                            {
                                break;
                            }
                        }
                        if(tcFactors.back() > halfOfTcAsInt)
                        {
                            tcFactors.pop_back();
                            tcFactor = tcFactors.end();
                        }
                        else
                        {
                            tcFactor = --(tcFactors.end());
                        }
                    }
                    if(tcFactor == tcFactors.end())
                    {
                        Coeff factor = result.coprimeFactor();
                        if(factor != (Coeff) 1)
                        {
                            result *= factor;
                            LOGMSG_TRACE("carl.core", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(result.mainVar(), std::initializer_list<Coeff>({(Coeff)1/factor})) << ")^" << 1 );
                            // Add the constant factor to the factors.
                            if( linearFactors.begin()->first.isConstant() )
                            {
                                factor = (Coeff)1 / factor;
                                factor *= linearFactors.begin()->first.lcoeff();
                                linearFactors.erase(linearFactors.begin());
                            }
                            linearFactors.insert(linearFactors.begin(), std::pair<UnivariatePolynomial<Coeff>, unsigned>(UnivariatePolynomial<Coeff>(result.mainVar(), std::initializer_list<Coeff>({factor})), 1));
                        }
                        return result;
                    }
                    lcFactor = lcFactors.begin();
                }
            }
        }
        assert(false);
    }
LinearFactorRemains:
    Coeff factor = result.lcoeff();
    if(factor != (Coeff) 1)
    {
        result /= factor;
        LOGMSG_TRACE("carl.core", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(result.mainVar(), factor) << ")^" << 1 );
        // Add the constant factor to the factors.
        if( linearFactors.begin()->first.isConstant() )
        {
            factor *= linearFactors.begin()->first.lcoeff();
            linearFactors.erase(linearFactors.begin());
        }
        linearFactors.insert(linearFactors.begin(), std::pair<UnivariatePolynomial<Coeff>, unsigned>(UnivariatePolynomial<Coeff>(result.mainVar(), factor), 1));
    }
    auto retVal = linearFactors.insert(std::pair<UnivariatePolynomial<Coeff>, unsigned>(result, 1));
    LOGMSG_TRACE("carl.core", "UnivELF: add the factor (" << result << ")^" << 1 );
    if(!retVal.second)
    {
        ++retVal.first->second;
    }
    return UnivariatePolynomial<Coeff>(result.mainVar(), (Coeff)1);
}

template<typename Coeff>
Coeff UnivariatePolynomial<Coeff>::syntheticDivision(const Coeff& zeroOfDivisor)
{
    if(coefficients().empty()) return Coeff(0);
    if(coefficients().size() == 1) return coefficients().back();
    std::vector<Coeff> secondRow;
    secondRow.reserve(coefficients().size());
    secondRow.push_back(Coeff(0));
    std::vector<Coeff> thirdRow(coefficients().size(), Coeff(0));
    size_t posThirdRow = coefficients().size()-1; 
    auto coeff = coefficients().rbegin();
    thirdRow[posThirdRow] = (*coeff) + secondRow.front();
    ++coeff;
    while(coeff != coefficients().rend())
    {
        secondRow.push_back(zeroOfDivisor*thirdRow[posThirdRow]);
        --posThirdRow;
        thirdRow[posThirdRow] = (*coeff) + secondRow.back();
        ++coeff;
    }
    assert(posThirdRow == 0);
    LOGMSG_TRACE("carl.core", "UnivSynDiv: (" << *this << ")[x -> " << zeroOfDivisor << "]  =  " << thirdRow.front());
    if(thirdRow.front() == 0)
    {
        thirdRow.erase(thirdRow.begin());
        this->mCoefficients.swap(thirdRow);
        LOGMSG_TRACE("carl.core", "UnivSynDiv: reduced by ((" << abs(getDenom(thirdRow.front())) << ")*" << mainVar() << " + (" << (thirdRow.front()<0 ? "-" : "") << abs(getNum(thirdRow.front())) << "))  ->  " << *this);
        return Coeff(0);
    }
    return thirdRow.front();
}

template<typename Coeff>
std::map<unsigned, UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::squareFreeFactorization() const
{
    LOGMSG_TRACE("carl.core", "UnivSSF: " << *this);
    std::map<unsigned,UnivariatePolynomial<Coeff>> result;
CLANG_WARNING_DISABLE("-Wtautological-compare")
	// degree() >= characteristic<Coeff>::value throws a warning in clang...
	if(characteristic<Coeff>::value != 0 && degree() >= characteristic<Coeff>::value)
CLANG_WARNING_RESET
    {
        LOGMSG_TRACE("carl.core", "UnivSSF: degree greater than characteristic!");
        result.insert(std::pair<unsigned, UnivariatePolynomial<Coeff>>(1, *this));
        LOGMSG_TRACE("carl.core", "UnivSSF: add the factor (" << *this << ")^1");
    }
    else
    {
        UnivariatePolynomial<Coeff> b = this->derivative();
        LOGMSG_TRACE("carl.core", "UnivSSF: b = " << b);
        UnivariatePolynomial<Coeff> s(mainVar());
        UnivariatePolynomial<Coeff> t(mainVar());
        UnivariatePolynomial<Coeff> c = extended_gcd((*this), b, s, t); // TODO: use gcd instead
        typename IntegralT<Coeff>::type numOfCpf = getNum(c.coprimeFactor());
        if(numOfCpf != 1) // TODO: is this maybe only necessary because the extended_gcd returns a polynomial with non-integer coefficients but it shouldn't?
        {
            c *= (Coeff) numOfCpf;
        }
        LOGMSG_TRACE("carl.core", "UnivSSF: c = " << c);
        if(c.isZero())
        {
            result.insert(std::pair<unsigned, UnivariatePolynomial<Coeff>>(1, *this));
            LOGMSG_TRACE("carl.core", "UnivSSF: add the factor (" << *this << ")^1");
        }
        else
        {
            UnivariatePolynomial<Coeff> w = (*this).divide(c).quotient;
            LOGMSG_TRACE("carl.core", "UnivSSF: w = " << w);
            UnivariatePolynomial<Coeff> y = b.divide(c).quotient;
            LOGMSG_TRACE("carl.core", "UnivSSF: y = " << y);
            UnivariatePolynomial<Coeff> z = y-w.derivative();
            LOGMSG_TRACE("carl.core", "UnivSSF: z = " << z);
            unsigned i = 1;
            while(!z.isZero())
            {
                LOGMSG_TRACE("carl.core", "UnivSSF: next iteration");
                UnivariatePolynomial<Coeff> g = extended_gcd(w, z, s, t); // TODO: use gcd instead
                numOfCpf = getNum(g.coprimeFactor());
                if(numOfCpf != 1) // TODO: is this maybe only necessary because the extended_gcd returns a polynomial with non-integer coefficients but it shouldn't?
                {
                    g *= (Coeff) numOfCpf;
                }
                LOGMSG_TRACE("carl.core", "UnivSSF: g = " << g);
                assert(result.find(i) == result.end());
                result.insert(std::pair<unsigned, UnivariatePolynomial<Coeff>>(i, g));
                LOGMSG_TRACE("carl.core", "UnivSSF: add the factor (" << g << ")^" << i);
                ++i;
                w = w.divide(g).quotient;
                LOGMSG_TRACE("carl.core", "UnivSSF: w = " << w);
                y = z.divide(g).quotient;
                LOGMSG_TRACE("carl.core", "UnivSSF: y = " << y);
                z = y - w.derivative();
                LOGMSG_TRACE("carl.core", "UnivSSF: z = " << z);
            }
            result.insert(std::pair<unsigned, UnivariatePolynomial<Coeff>>(i, w));
            LOGMSG_TRACE("carl.core", "UnivSSF: add the factor (" << w << ")^" << i);
        }
    }
    return result;
}

template<typename Coeff>
void UnivariatePolynomial<Coeff>::eliminateZeroRoots() {
	unsigned int i = 0;
	while ((i < this->mCoefficients.size()-1) && (this->mCoefficients[i] == 0)) i++;
	if (i == 0) return;
	// Now shift by i elements, drop lower i coefficients (they are zero anyway)
	for (unsigned int j = 0; j < this->mCoefficients.size()-i; j++) {
		this->mCoefficients[i] = this->mCoefficients[j+i];
	}
	this->mCoefficients.resize(this->mCoefficients.size()-i);
}

template<typename Coeff>
void UnivariatePolynomial<Coeff>::eliminateRoot(const Coeff& root) {
	if (root == 0) {
		this->eliminateZeroRoots();
		return;
	}
	do {
		std::vector<Coeff> tmp(this->mCoefficients.size()-1);
		for (unsigned long i = this->mCoefficients.size()-1; i > 0; i--) {
			tmp[i-1] = this->mCoefficients[i];
			this->mCoefficients[i-1] += this->mCoefficients[i] * root;
		}
		this->mCoefficients = tmp;
	} while ((this->evaluate(root) == 0) && (this->mCoefficients.size() > 0));
}

template<typename Coeff>
std::list<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::standardSturmSequence() const {
	return this->standardSturmSequence(this->derivative());
}

template<typename Coeff>
std::list<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::standardSturmSequence(const UnivariatePolynomial<Coeff>& polynomial) const {
	assert(this->mainVar() == polynomial.mainVar());

	std::list<UnivariatePolynomial<Coeff>> seq;

	UnivariatePolynomial<Coeff> p = *this;
	UnivariatePolynomial<Coeff> q = polynomial;

	seq.push_back(p);
	while (! q.isZero()) {
		seq.push_back(q);
		q = - p.reduce(q);
		p = seq.back();
	}
	return seq;
}

template<typename Coeff>
unsigned int UnivariatePolynomial<Coeff>::signVariations(const ExactInterval<Coeff>& interval) const {
	UnivariatePolynomial<Coeff> p(*this);
	p.shift(interval.left());
	p.scale(interval.diameter());
	p.reverse();
	p.shift(1);
	return carl::signVariations(p.mCoefficients.begin(), p.mCoefficients.end(), [](const Coeff& c){ return carl::sgn(c); });
}

template<typename Coeff>
unsigned int UnivariatePolynomial<Coeff>::countRealRoots(const ExactInterval<Coeff>& interval) const {
	auto seq = this->standardSturmSequence();
	unsigned int l = carl::signVariations(seq.begin(), seq.end(), [&interval](const UnivariatePolynomial<Coeff>& p){ return p.sgn(interval.left()); });
	unsigned int r = carl::signVariations(seq.begin(), seq.end(), [&interval](const UnivariatePolynomial<Coeff>& p){ return p.sgn(interval.right()); });
	return l - r;
}


template<typename Coeff>
void UnivariatePolynomial<Coeff>::reverse() {
	std::reverse(this->mCoefficients.begin(), this->mCoefficients.end());
}

template<typename Coeff>
void UnivariatePolynomial<Coeff>::scale(const Coeff& factor) {
	Coeff f = factor;
	for (unsigned int i = 1; i < this->mCoefficients.size(); i++) {
		this->mCoefficients[i] *= f;
		f *= factor;
	}
}

template<typename Coeff>
void UnivariatePolynomial<Coeff>::shift(const Coeff& a) {
	std::vector<Coeff> next;
	next.reserve(this->mCoefficients.size());
	next.push_back(this->mCoefficients.back());

	for (unsigned int i = 0; i < this->mCoefficients.size()-1; i++) {
		next.push_back(next.back());
		for (unsigned int j = i; j > 0; j--) {
			next[j] = a * next[j] + next[j-1];
		}
		next[0] = a * next[0] + this->mCoefficients[this->mCoefficients.size()-2-i];
	}
	this->mCoefficients = next;
}

template<typename Coeff>
const std::list<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::subresultants(
		const UnivariatePolynomial<Coeff>& p,
		const UnivariatePolynomial<Coeff>& q,
		const SubresultantStrategy strategy
) {
	/* The algorithm consists of three parts:
	 * Part 1: Initialization, i.e. preparation1 of the input so that the requirements of the core algorithm in parts 2 and 3 are met.
	 * Part 2: First part of the main loop. If the two subresultants which were added before (initially the two inputs) differ by more
	 *         than 1 in their degree, an intermediate subresultant is computed by reducing the last one added with the leading coefficient
	 *         of the one before this one.
	 * Part 3: Second part of the main loop. The pseudo remainder of the last two subresultants (the one possibly added in Part 2 disregarded)
	 *         is computed and added to the subresultant sequence.
	 */

	/* Part 1
	 * Check and normalize input, initialize local variables.
	 */
	
	assert(p.mainVar() == q.mainVar());
	std::list<UnderlyingNumberType<Coeff>> subresultants;
	Variable variable = p.mainVar();
	
	// a shall receive the smaller-degree polynomial
	UnivariatePolynomial<Coeff> a, b;
	
	// aDeg >= bDeg shall hold, so switch if it does not hold
	if (p.degree() < q.degree()) {
		a = q;
		b = p;
	} else {
		a = p;
		b = q;
	}
	
	subresultants.push_front(a);
	if (b.isZero()) return subresultants;
	subresultants.push_front(b);
	
	// SPECIAL CASE: both, a and b, are constant
	if (b.isConstant()) return subresultants;
	
	UnivariatePolynomial<Coeff> tmp = b;
	// TODO: check if reduce() is really prem()
	b = a.reduce(-b);
	a = tmp;
	
	// BUG in Duco's article(?):
	//ex subresLcoeff = GiNaC::pow( a.lcoeff(), a.degree() - b.degree() );    // initialized on the basis of the smaller-degree polynomial
	Coeff subresLcoeff = a.lcoeff(); // initialized on the basis of the smaller-degree polynomial
	
	/* Parts 2 and 3
	 * Main loop filling the subresultants chain step by step.
	 */
	// MAIN: start main loop containing different computation strategies
	while (true) {
		if (b.isZero()) return subresultants;
		unsigned aDeg = a.degree();
		unsigned bDeg = b.degree();
		subresultants.push_front(b);
		
		// Part 2
		unsigned delta = aDeg - bDeg;
		
		/** Case distinction on delta: either we choose b as next subresultant or we could reduce b (delta > 1)
		 * and add the reduced version c as next subresultant. The reduction is done by division, which
		 * depends on the internal variable order of GiNaC and might fail although for some order it would succeed.
		 * In this case, we just do not reduce b. (A relaxed reduction could also be applied.)
		 *
		 * After the if-else block, bDeg is the degree of the front-most element of subresultants, be it c or b.
		 */
		UnivariatePolynomial<Coeff> c;
		if (delta > 1) {
			// compute c
			// Notation hints: Compared to [Duc98], here S_{d-1} is b and S_d is a, and S_e is c.
			switch (strategy) {
				case SubresultantStrategy::Generic: {
					UnivariatePolynomial<Coeff> reductionCoeff((b.lcoeff().pow(delta - 1) * b).toUnivariate(variable));
					UnivariatePolynomial<Coeff> dividant(MultivariatePolynomial<Coeff>(subresLcoeff).pow(delta-1).toUnivariatePolynomial(variable));
					DivisionResult<UnivariatePolynomial<Coeff>> res = reductionCoeff.divide(dividant);
					if (res.remainder.isZero()) {
						c = res.quotient;
						subresultants.push_front(c);
						bDeg = c.degree();
					} else {
						c = b;
					}
					break;
				}
				case SubresultantStrategy::Ducos:
				case SubresultantStrategy::Lazard: {
					// "dichotomous Lazard": efficient exponentiation
					unsigned deltaReduced = delta-1;
					// should be true by the loop condition
					assert(deltaReduced > 0);
					
					Coeff lcoeffB = b.lcoeff();
					UnivariatePolynomial<Coeff> reductionCoeff(variable, lcoeffB);
					
					unsigned exponent = highestPower(deltaReduced);
					deltaReduced -= exponent;
					
					while (exponent != 1) {
						exponent /= 2;
						auto res = (reductionCoeff*reductionCoeff).divide(subresLcoeff);
						if (res.remainder.isZero() && deltaReduced >= exponent) {
							auto res2 = (res.quotient*lcoeffB).divide(subresLcoeff);
							reductionCoeff = res2.quotient;
							deltaReduced -= exponent;
						}
					}
					reductionCoeff *= b;
					auto res = reductionCoeff.divide(subresLcoeff);
					if (res.remainder.isZero()) {
						c = res.quotient;
						subresultants.push_front(c);
						bDeg = c.degree();
					} else {
						c = b;
					}
					break;
				}
			}
		} else {
			c = b;
		}
		if (bDeg == 0) return subresultants;
		
		// Part 3
		switch (strategy) {
			// Compared to [Duc98], here S_{d-1} is b and S_d is a, S_e is c, and s_d is subresLcoeff.
			case SubresultantStrategy::Generic:
			case SubresultantStrategy::Lazard: {
				if (a.isZero()) return subresultants;
				
				/* If b was constant, the degree properties for subresultants are still met, enforcing us to disregard whether
				 * the above division was successful (in this case, reducedNewB remains unchanged).
				 * If it was successful, the resulting term is safely added to the list, yielding an optimized resultant.
				 */
				// TODO: check if reduce() is really prem()
				UnivariatePolynomial<Coeff> reducedNewB = a.reduce(-b);
				auto res = reducedNewB.divide(subresLcoeff.pow(delta)*a.lcoeff());
				b = res.quotient;
				break;
			}
			case SubresultantStrategy::Ducos: {
				// Ducos' optimization
				Coeff lcoeffB = b.lcoeff();
				Coeff lcoeffC = c.lcoeff();
				std::vector<Coeff> h(aDeg);
				
				for (unsigned d = 0; d < bDeg; d++) {
					h[d] = lcoeffC * Coeff(variable).pow(d);
				}
				if (aDeg != bDeg) { // => aDeg > bDeg
					h[bDeg] = lcoeffC * Coeff(variable).pow(bDeg) - c; // H_e
				}
				for (unsigned d = bDeg + 1; d < aDeg; d++) {
					Coeff t = h[d-1] * variable;
					Coeff reducedNewB(t.toUnivariatePolynomial(variable).coefficients()[bDeg] * b);
					auto res = reducedNewB.divide(lcoeffB);
					reducedNewB = res.quotient;
					assert(res.quotient.isZero() || reducedNewB.degree() == 0);
					h[d] = t - reducedNewB;
				}
				
				Coeff sum(h.front() * a.coefficients()[0]);
				for (unsigned d = 1; d < aDeg; d++) {
					sum += h[d] * a.coefficients()[d];
				}
				Coeff normalizedSum;
				auto res = sum.divide(a.lcoeff());
				normalizedSum = res.quotient;
				assert(res.remainder.isZero() || sum.degree() == 0);
				
				Coeff t(h.back() * variable);
				UnivariatePolynomial<Coeff> reducedNewB(((t + normalizedSum) * lcoeffB - t.coefficients()[bDeg]).toUnivariatePolynomial(variable));
				auto res2 = reducedNewB.divide(a.lcoeff());
				reducedNewB = res2.quotient;
				if (delta % 2 == 0) {
					b = -reducedNewB;
				} else {
					b = reducedNewB;
				}
				break;
			}
		}
		a = c;
		subresLcoeff = a.lcoeff();
	}
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::operator -() const
{
	UnivariatePolynomial result(mMainVar);
	result.mCoefficients.reserve(mCoefficients.size());
	for(auto c : mCoefficients)
	{
		result.mCoefficients.push_back(-c);
	}

	return result;		 
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator+=(const Coeff& rhs)
{
	if(rhs == 0) return *this;
	if(mCoefficients.empty())
	{
		// Adding non-zero rhs to zero.
		mCoefficients.resize(1, rhs);
	}
	else
	{
		mCoefficients.front() += rhs;
		if(mCoefficients.size() == 1 && mCoefficients.front() == (Coeff)0) 
		{
			// Result is zero.
			mCoefficients.clear();
		}
	}
	return *this;
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator+=(const UnivariatePolynomial& rhs)
{
	assert(mMainVar == rhs.mMainVar);
	
	if(rhs.isZero())
	{
		return *this;
	}
	
	if(mCoefficients.size() < rhs.mCoefficients.size())
	{
		for(unsigned i = 0; i < mCoefficients.size(); ++i)
		{
			mCoefficients[i] += rhs.mCoefficients[i];
		}
		mCoefficients.insert(mCoefficients.end(), rhs.mCoefficients.end() - ((unsigned)(rhs.mCoefficients.size() - mCoefficients.size())), rhs.mCoefficients.end());
	}
	else
	{
		for(unsigned i = 0; i < rhs.mCoefficients.size(); ++i)
		{
			mCoefficients[i] += rhs.mCoefficients[i]; 
		}
	}
	stripLeadingZeroes();
	return *this;
}

template<typename C>
UnivariatePolynomial<C> operator+(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res += rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator+(const UnivariatePolynomial<C>& lhs, const C& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res += rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator+(const C& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs + lhs;
}
	

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator-=(const Coeff& rhs)
{
	LOG_INEFFICIENT();
	return *this += -rhs;
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator-=(const UnivariatePolynomial& rhs)
{
	LOG_INEFFICIENT();
	return *this += -rhs;
}


template<typename C>
UnivariatePolynomial<C> operator-(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res -= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator-(const UnivariatePolynomial<C>& lhs, const C& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res -= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator-(const C& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs - lhs;
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator*=(const Coeff& rhs)
{
	if(rhs == 0)
	{
		mCoefficients.clear();
		return *this;
	}
	for(Coeff& c : mCoefficients)
	{
		c *= rhs;
	}
	
	if(is_finite_domain<Coeff>::value)
	{
		stripLeadingZeroes();
	}
	
	return *this;		
}


template<typename Coeff>
template<typename I, DisableIf<std::is_same<Coeff, I>>...>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator*=(const typename IntegralT<Coeff>::type& rhs)
{
	static_assert(std::is_same<Coeff, I>::value, "Do not provide template parameters");
	if(rhs == (I)0)
	{
		mCoefficients.clear();
		return *this;
	}
	for(Coeff& c : mCoefficients)
	{
		c *= rhs;
	}
	return *this;		
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator*=(const UnivariatePolynomial& rhs)
{
	assert(mMainVar == rhs.mMainVar);
	if(rhs.isZero())
	{
		mCoefficients.clear();
		return *this;
	}
	
	std::vector<Coeff> newCoeffs; 
	newCoeffs.reserve(mCoefficients.size() + rhs.mCoefficients.size());
	for(unsigned e = 0; e < mCoefficients.size() + rhs.degree(); ++e)
	{
		newCoeffs.push_back((Coeff)0);
		for(unsigned i = 0; i < mCoefficients.size() && i <= e; ++i)
		{
			if(e - i < rhs.mCoefficients.size())
			{
				newCoeffs.back() += mCoefficients[i] * rhs.mCoefficients[e-i];
			}
		}
	}
	mCoefficients.swap(newCoeffs);
	stripLeadingZeroes();
	return *this;
}


template<typename C>
UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res *= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const C& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res *= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator*(const C& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs * lhs;
}

template<typename C>
UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const typename IntegralT<C>::type& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res *= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator*(const typename IntegralT<C>::type& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs * lhs;
}


template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator/=(const Coeff& rhs)
{
	if(!is_field<Coeff>::value)
	{
		LOGMSG_WARN("carl.core", "Division by coefficients is only defined for field-coefficients");
	}
	assert(rhs != 0);
	for(Coeff& c : mCoefficients)
	{
		c /= rhs;
	}
	return *this;		
}

template<typename C>
UnivariatePolynomial<C> operator/(const UnivariatePolynomial<C>& lhs, const C& rhs)
{
	static_assert(is_field<C>::value, "Division by coefficients is only defined for field-coefficients");
	assert(rhs != 0);
	UnivariatePolynomial<C> res(lhs);
	return res /= rhs;
}

template<typename C>
bool operator==(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	if(lhs.mMainVar == rhs.mMainVar)
	{
		return lhs.mCoefficients == rhs.mCoefficients;
	}
	else
	{
		// in different variables, polynomials can still be equal if constant.
		if(lhs.isZero() && rhs.isZero()) return true;
		if(lhs.isConstant() && rhs.isConstant() && lhs.lcoeff() == rhs.lcoeff()) return true;
		return false;
	}
}
template<typename C>
bool operator==(const UnivariatePolynomialPtr<C>& lhs, const UnivariatePolynomialPtr<C>& rhs)
{
	if (lhs == nullptr && rhs == nullptr) return true;
	if (lhs == nullptr || rhs == nullptr) return false;
	return *lhs == *rhs;
}

template<typename C>
bool operator==(const UnivariatePolynomial<C>& lhs, const C& rhs)
{	
	if(lhs.isZero())
	{
		return rhs == 0;
	}
	if(lhs.isConstant() && lhs.lcoeff() == rhs) return true;
	return false;
}

template<typename C>
bool operator==(const C& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs == lhs;
}


template<typename C>
bool operator!=(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	return !(lhs == rhs);
}
template<typename C>
bool operator!=(const UnivariatePolynomialPtr<C>& lhs, const UnivariatePolynomialPtr<C>& rhs)
{
	if (lhs == nullptr && rhs == nullptr) return false;
	if (lhs == nullptr || rhs == nullptr) return true;
	return *lhs != *rhs;
}

template<typename C>
bool UnivariatePolynomial<C>::less(const UnivariatePolynomial<C>& rhs, const PolynomialComparisonOrder& order) const {
	switch (order) {
		case PolynomialComparisonOrder::CauchyBound: /*{
			C a = this->cauchyBound();
			C b = rhs.cauchyBound();
			if (a < b) return true;
			return (a == b) && this->less(rhs);
		}*/
		case PolynomialComparisonOrder::LowDegree:
			if (this->degree() < rhs.degree()) return true;
			return (this->degree() == rhs.degree()) && this->less(rhs);
		case PolynomialComparisonOrder::Memory:
			return this < &rhs;
	}
}
template<typename C>
bool less(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs, const PolynomialComparisonOrder& order = PolynomialComparisonOrder::Default)
{
	return lhs.less(rhs, order);
}
template<typename C>
bool less(const UnivariatePolynomial<C>* lhs, const UnivariatePolynomial<C>* rhs, const PolynomialComparisonOrder& order = PolynomialComparisonOrder::Default)
{
	if (lhs == nullptr) return rhs != nullptr;
	if (rhs == nullptr) return true;
	return lhs->less(*rhs, order);
}
template<typename C>
bool less(const UnivariatePolynomialPtr<C>& lhs, const UnivariatePolynomialPtr<C>& rhs, const PolynomialComparisonOrder& order = PolynomialComparisonOrder::Default)
{
	return less(lhs.get(), rhs.get(), order);
}

template<typename C>
class UnivariatePolynomialComparator
{
private:
	PolynomialComparisonOrder order;
public:
	UnivariatePolynomialComparator(PolynomialComparisonOrder order = PolynomialComparisonOrder::Default)
				: order(order)
	{}

	bool operator()(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs) const
	{
		return less(lhs, rhs, order);
	}
	bool operator()(const UnivariatePolynomial<C>* lhs, const UnivariatePolynomial<C>* rhs) const
	{
		return less(lhs, rhs, order);
	}
};

template<typename C>
bool operator<(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	if(lhs.mMainVar == rhs.mMainVar)
	{
		if(lhs.coefficients().size() == rhs.coefficients().size())
        {
            auto iterLhs = lhs.coefficients().rbegin();
            auto iterRhs = rhs.coefficients().rbegin();
            while(iterLhs != lhs.coefficients().rend())
            {
                assert(iterRhs != rhs.coefficients().rend());
                if(*iterLhs == *iterRhs)
                {
                    ++iterLhs;
                    ++iterRhs;
                }
                else
                {
                    return *iterLhs < *iterRhs;
                }
            }
        }
		return lhs.coefficients().size() < rhs.coefficients().size();
	}
    return lhs.mMainVar < rhs.mMainVar;
}

template<typename C>
std::ostream& operator<<(std::ostream& os, const UnivariatePolynomial<C>& rhs)
{
	if(rhs.isZero()) return os << "0";
	for(size_t i = 0; i < rhs.mCoefficients.size()-1; ++i )
	{
		const C& c = rhs.mCoefficients[rhs.mCoefficients.size()-i-1];
		if(c != 0)
		{
			os << "(" << c << ")*" << rhs.mMainVar << "^" << rhs.mCoefficients.size()-i-1 << " + ";
		}
	}
	os << rhs.mCoefficients[0];
	return os;
}
}
