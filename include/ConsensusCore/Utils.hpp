// Author: David Alexander

#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <boost/foreach.hpp>
#include <string>

#include <ConsensusCore/Types.hpp>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define LOCATION __FILE__ ":" TOSTRING(__LINE__)

// Floating point comparison
bool AlmostEqual(float A, float B, int maxUlps = 4);

#define ShouldNotReachHere()                                     \
    fprintf(stderr, "Should not reach here! at " LOCATION "\n"); \
    throw InternalError("Should not reach here: " LOCATION)

#define NotYetImplemented() throw NotYetImplementedException()

// For use in debugging only.
#define Breakpoint() asm volatile("int3;")

#ifdef NDEBUG
#define DEBUG_ONLY(stmt)
#else
#define DEBUG_ONLY(stmt) stmt
#endif

// Workarounds for Eclipse+CDT problems
#ifdef __CDT_PARSER__
#define foreach(a, b) for (a : b)
#else
#define foreach(a, b) BOOST_FOREACH (a, b)
#endif

// Aggressive sledghammer for forcing inlining
#ifdef _MSC_VER
#define INLINE_CALLEES
#else
#define INLINE_CALLEES __attribute__((flatten))
#endif
