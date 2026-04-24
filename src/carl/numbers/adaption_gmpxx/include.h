#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

#include <cstddef>

// Disable potential warning on newer AppleClang versions
#if __GNUC__ && defined(__has_warning)
#if __has_warning("-Wdeprecated-literal-operator")
#define SUPPRESSING
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-literal-operator"
#endif
#endif

#include <gmpxx.h>

#ifdef SUPPRESSING
#undef SUPPRESSING
#pragma GCC diagnostic pop
#endif
