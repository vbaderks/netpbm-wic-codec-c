// Copyright (c) Team CharLS.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <stdio.h>
#include <assert.h>

#define SUPPRESS_WARNING_NEXT_LINE(x) \
    __pragma(warning(suppress \
                     : x)) // NOLINT(misc-macro-parentheses, bugprone-macro-parentheses, cppcoreguidelines-macro-usage)

#ifdef NDEBUG

#define ASSERT(expression) (void)(0)
#define VERIFY(expression) (void)(expression)

#else

#define ASSERT(expression) \
    __pragma(warning(push)) __pragma(warning(disable : 26493)) assert(expression) __pragma(warning(pop))
#define VERIFY(expression) assert(expression)

#endif

// The TRACE macro can be used in debug build to watch the behaviour of the implementation
// when used by 3rd party applications.
#ifdef NDEBUG
    #define TRACE __noop
#else
    #define TRACE(format, ...) \
    do { \
        char buffer[256]; \
        snprintf(buffer, sizeof(buffer), format __VA_OPT__(,) __VA_ARGS__); \
        OutputDebugStringA(buffer); \
    } while(0)
#endif
