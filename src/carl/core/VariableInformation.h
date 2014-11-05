/** 
 * @file   VariableInformation.h
 * @ingroup multirp
 * @author Sebastian Junges
 * @since September 3, 2013
 */

#pragma once
#include <map>
#include <vector>
#include <memory>
#include <algorithm>

#include "Variable.h"
namespace carl
{

	template<bool collectCoeff, typename CoeffType>
	struct VariableInformation
	{
		
	};

	template<typename CoeffType>
	class VariableInformation<false, CoeffType>
	{
		
		/// Maximal degree variable occurs with.
		unsigned mMaxDegree;
		/// Minimal non-zero degree variable occurs with.
		unsigned mMinDegree;
		/// Number of terms a variable occurs in.
		unsigned mOccurence;
	public:
		VariableInformation()
		:	mMaxDegree(0), 
			mMinDegree(0), 
			mOccurence(0)
		{
			
		}
        
		VariableInformation(unsigned degreeOfOccurence)
		:	mMaxDegree(degreeOfOccurence), 
			mMinDegree(degreeOfOccurence), 
			mOccurence(1)
		{
			
		}
        
		VariableInformation(const VariableInformation<false, CoeffType>& varInfo)
		:	mMaxDegree(varInfo.mMaxDegree), 
			mMinDegree(varInfo.mMinDegree), 
			mOccurence(varInfo.mOccurence)
		{
			
		}
        
        bool hasCoeff() const
        {
            return false;
        }
    
		unsigned maxDegree() const
		{
			return mMaxDegree;
		}
		
		unsigned minDegree() const
		{
			return mMinDegree;
		}
		
		unsigned occurence() const
		{
			return mOccurence;
		}
		
		/**
		 * If degree is larger than maxDegree, we set the maxDegree to degree.
         * @param degree
		 * @return true if degree was larger.
         */
		bool raiseMaxDegree(unsigned degree)
		{
			if(mMaxDegree < degree)
			{
				mMaxDegree = degree;
				return true;
			}
			return false;
		}
		
		/**
		 * If degree is smaller than minDegree, we set the minDegree to degree.
         * @param degree
		 * @return true if degree was smaller.
         */
		bool lowerMinDegree(unsigned degree)
		{
			if(mMinDegree > degree)
			{
				mMinDegree = degree;
				return true;
			}
			return false;
		}
		
		
		void increaseOccurence()
		{
			++mOccurence;
		}
		
		template<typename Term>
		void updateCoeff(unsigned, const Term&)
		{
			// Empty function, we do not save the coefficient here.
			// TODO there might be a better solution to this.
		}
        
        void collect(Variable::Arg var, const typename CoeffType::CoeffType&, const typename CoeffType::MonomType& monomial)
        {
            exponent e = monomial.exponentOfVariable(var);
            if(e > 0)
            {
                // One more term in which the variable occurs.
                increaseOccurence();
                // Update minimal/maximal degree.
                if(!raiseMaxDegree(e))
                {
                    // Only if raising failed, lowering can be successful.
                    lowerMinDegree(e);
                }
            }
        }
	};
	
	template<typename CoeffType>
	class VariableInformation<true, CoeffType> : public VariableInformation<false, CoeffType>
	{
		std::map<unsigned, CoeffType> mCoeffs;
	public:
		VariableInformation() :  VariableInformation<false, CoeffType>()
		{
		}
        
		VariableInformation(unsigned degreeOfOccurence) :  VariableInformation<false, CoeffType>(degreeOfOccurence)
		{
		}
        
		VariableInformation(const VariableInformation<false, CoeffType>& varInfo) :  VariableInformation<false, CoeffType>(varInfo)
        {
        }
        
        bool hasCoeff() const
        {
            return !mCoeffs.empty();
        }
	
		const std::map<unsigned, CoeffType>& coeffs() const
		{
			return mCoeffs;
		}
		
		template<typename Term>
		void updateCoeff(unsigned exponent, const Term& t)
		{
			typename std::map<unsigned, CoeffType>::iterator it = mCoeffs.find(exponent);
			if(it == mCoeffs.end())
			{
				mCoeffs.emplace(exponent, CoeffType(t));
				
			}
			else
			{
				it->second += t;
			}
		}
        
        void collect(Variable::Arg v, const typename CoeffType::CoeffType& termCoeff, const typename CoeffType::MonomType& monomial)
        {
            exponent e = 0;
            std::vector<std::pair<Variable, exponent>> exps;
            exps.reserve(monomial.nrVariables()-1);
            exponent totalDegree = monomial.tdeg();
            for(unsigned i = 0; i<monomial.nrVariables(); ++i)
            {
                if(monomial[i].first == v)
                {
                    totalDegree -= monomial[i].second;
                    e = monomial[i].second;
                }
                else
                {
                    exps.push_back(monomial[i]);
                }
            }
            if(totalDegree != 0) 
            { 
                std::shared_ptr<typename CoeffType::MonomType> m = std::make_shared<typename CoeffType::MonomType>(std::move(exps), totalDegree);
                updateCoeff(e, typename CoeffType::TermType(termCoeff, m));
            }
            else
            {
                updateCoeff(e, typename CoeffType::TermType(termCoeff));
            }
            if(e > 0)
            {
                // One more term in which the variable occurs.
                this->increaseOccurence();
                // Update minimal/maximal degree.
                if(!this->raiseMaxDegree(e))
                {
                    // Only if raising failed, lowering can be successful.
                    this->lowerMinDegree(e);
                }
            }
        }
	};

}
	
