/**
 * @file IDGenerator.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <mutex>
#include <queue>
#include <stack>

#include "../config.h"

namespace carl {

#define SMART_ID_GENERATOR
#ifdef SMART_ID_GENERATOR

class IDGenerator {
   private:
    std::size_t mNext = 1;
    std::priority_queue<std::size_t> mFree;
#ifdef THREAD_SAFE
    mutable std::mutex mMutex;
#endif
   public:
    IDGenerator() = default;
    std::size_t get() {
#ifdef THREAD_SAFE
        std::lock_guard<std::mutex> lock(mMutex);
#endif
        std::size_t res = mNext;
        if (mFree.empty())
            ++mNext;
        else {
            res = mFree.top();
            mFree.pop();
        }
        return res;
    }

    void free(std::size_t id) {
        assert(id > 0);
#ifdef THREAD_SAFE
        std::lock_guard<std::mutex> lock(mMutex);
#endif
        if (id == mNext - 1) {
            --mNext;
            while (!mFree.empty()) {
                if (mFree.top() != mNext - 1)
                    break;
                --mNext;
                mFree.pop();
            }
        } else {
            mFree.push(id);
        }
    }
    std::size_t nextID() const {
#ifdef THREAD_SAFE
        std::lock_guard<std::mutex> lock(mMutex);
#endif
        return mNext;
    }
    void clear() {
#ifdef THREAD_SAFE
        std::lock_guard<std::mutex> lock(mMutex);
#endif
        mNext = 1;
        mFree = std::priority_queue<std::size_t>();
    }
};

#else

class IDGenerator {
   private:
    std::size_t mNext = 1;
    std::stack<std::size_t> mFree;

   public:
    IDGenerator() = default;
    std::size_t get() {
        std::size_t res = mNext;
        if (mFree.empty())
            ++mNext;
        else {
            res = mFree.top();
            mFree.pop();
        }
        return res;
    }

    void free(std::size_t id) {
        mFree.push(id);
    }
    std::size_t nextID() const {
        return mNext;
    }
    void clear() {
        mNext = 1;
        mFree.clear();
    }
};

#endif

}  // namespace carl
