/*
 * GiNaCRA - GiNaC Real Algebra package
 * Copyright (C) 2010-2012  Ulrich Loup, Joachim Redies, Sebastian Junges
 *
 * This file is part of GiNaCRA.
 *
 * GiNaCRA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GiNaCRA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GiNaCRA.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * @file CriticalPairs.h
 * @ingroup gb
 * @author Sebastian Junges
 */
#pragma once

#include "../../core/CompareResult.h"
#include "../../core/MonomialOrdering.h"
#include "../../util/Heap.h"
#include "CriticalPairsEntry.h"

#include <unordered_map>

namespace carl {

template<class Compare>
class CriticalPairConfiguration {
   public:
    using Entry = CriticalPairsEntry<Compare>*;
    using CompareResult = carl::CompareResult;

    static CompareResult compare(Entry e1, Entry e2) {
        return Compare::compare(e1->getSortedFirstLCM(), e2->getSortedFirstLCM());
    }

    static bool cmpLessThan(CompareResult res) {
        return res == CompareResult::GREATER;
    }
    static const bool supportDeduplicationWhileOrdering = false;

    static bool cmpEqual(CompareResult res) {
        return res == CompareResult::EQUAL;
    }

    using Order = Compare;
    static const bool fastIndex = true;
};

/**
 * A data structure to store all the SPolynomial pairs which have to be checked.
 */
template<template<class> class Datastructure, class Configuration>
class CriticalPairs {
   public:
    CriticalPairs() : mDatastruct(Configuration()) {}

    /**
     * Add a list of s-pairs to the list.
     * @param pairs
     */
    void push(std::list<SPolPair> pairs) {
        if (pairs.empty())
            return;
        mDatastruct.push(new CriticalPairsEntry<typename Configuration::Order>(std::move(pairs)));
    }

    /**
     * Gets the first SPol from the data structure and removes it from the data structure.
     * @return
     */
    SPolPair pop();
    /**
     * Eliminate multiples of the given monomial.
     * @param lm
     * @param newpairs
     */
    void elimMultiples(const Monomial::Arg& lm, const std::unordered_map<size_t, SPolPair>& newpairs);

    /**
     * Checks whether there are any pairs in the data structure.
     * @return
     */
    bool empty() const {
        return mDatastruct.empty();
    }

    /**
     * Print the underlying data structure.
     */
    void print() const {
        mDatastruct.print();
    }

    /**
     * Checks the size of the data structure.
     * Please notice that this is not necessarily the number of pairs in the data structure, as the underlying elements may be lists themselves.
     * @return
     */
    unsigned size() const {
        return mDatastruct.size();
    }

   private:
    Datastructure<Configuration> mDatastruct;
};

typedef CriticalPairs<Heap, CriticalPairConfiguration<GrLexOrdering>> CritPairs;

}  // namespace carl

#include "CriticalPairs.tpp"
