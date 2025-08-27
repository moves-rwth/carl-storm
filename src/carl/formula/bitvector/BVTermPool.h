/*
 * File:   BVTermPool.h
 * Author: Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "BVTerm.h"
#include "Pool.h"

namespace carl {
class BVTermPool : public Singleton<BVTermPool>, public Pool<BVTermContent> {
    friend Singleton<BVTermPool>;

   public:
    using Term = BVTermContent;
    using TermPtr = Term*;
    using ConstTermPtr = const Term*;

   private:
    ConstTermPtr mpInvalid;

   public:
    BVTermPool();
    BVTermPool(const BVTermPool&) = delete;
    BVTermPool& operator=(const BVTermPool&) = delete;

    ConstTermPtr create();

    ConstTermPtr create(BVTermType _type, BVValue&& _value);

    ConstTermPtr create(BVTermType _type, const BVVariable& _variable);

    ConstTermPtr create(BVTermType _type, const BVTerm& _operand, std::size_t _index = 0);

    ConstTermPtr create(BVTermType _type, const BVTerm& _first, const BVTerm& _second);

    ConstTermPtr create(BVTermType _type, const BVTerm& _operand, std::size_t _first, std::size_t _last);

    void assignId(TermPtr _term, std::size_t _id) override;
};
}  // namespace carl

#define BV_TERM_POOL BVTermPool::getInstance()
