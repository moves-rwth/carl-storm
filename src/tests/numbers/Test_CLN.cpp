#include <gtest/gtest.h>
#include <carl/core/logging.h>
#include <carl/numbers/numbers.h>

#ifndef CARL_THREAD_SAFE
#ifdef CARL_USE_CLN_NUMBERS
TEST(CLN, Rationalize)
{
    EXPECT_TRUE( carl::rationalize<cln::cl_RA>(carl::toDouble(cln::cl_RA(1)/cln::cl_RA(3))) != cln::cl_RA(1)/cln::cl_RA(3) );
    EXPECT_TRUE( carl::rationalize<cln::cl_RA>(carl::toDouble(cln::cl_RA(1)/cln::cl_RA(20))) != cln::cl_RA(1)/cln::cl_RA(20) );
}
#endif
#endif
