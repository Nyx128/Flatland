#pragma once
#include <stdint.h>
#include "logger.hpp"

#define FL_ASSERTIONS_ENABLED

#ifdef FL_ASSERTIONS_ENABLED

void report_assertion_failure(const char* expression, const char* message, const char* file, uint32_t line);

#ifndef FL_ASSERT(expr)

#define FL_ASSERT(expr)													\
    {																	\
        if (expr) {														\
        } else {														\
            report_assertion_failure(#expr, "", __FILE__, __LINE__);	\
            __debugbreak();												\
        }																\
    }
#endif

#ifndef FL_ASSERT_MSG(expr, message)

#define FL_ASSERT_MSG(expr, message)									    \
    {																	    \
        if (expr) {														    \
        } else {														    \
            report_assertion_failure(#expr, message, __FILE__, __LINE__);	\
            __debugbreak();												    \
        }																    \
    }
#endif // !FL_ASSERT_MSG(expr, message)


#ifdef _DEBUG

#ifndef FL_ASSERT_DEBUG(expr)
#define FL_ASSERT_DEBUG(expr)													\
    {																	        \
        if (expr) {														        \
        } else {														        \
            report_assertion_failure(#expr, "", __FILE__, __LINE__);	        \
            __debugbreak();												        \
        }																        \
    }
#else
#define FL_ASSERT_DEBUG(expr)
#endif
#endif
#else
#define FL_ASSERT(expr)
#define FL_ASSERT_MSG(expr, message)
#define FL_ASSERT_DEBUG(expr)
#endif