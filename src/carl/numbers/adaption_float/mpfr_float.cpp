#include "../numbers.h"

#ifdef CARL_USE_MPFR_FLOAT
carl::precision_t carl::FLOAT_T<mpfr_t>::mDefaultPrecision = 53;
#endif
