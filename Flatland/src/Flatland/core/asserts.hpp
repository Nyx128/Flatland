#pragma once
#include <stdint.h>
#include "logger.hpp"

#define FL_ASSERTIONS_ENABLED

#ifdef FL_ASSERTIONS_ENABLED

void report_assertion_failure(const char* expression, const char* message, const char* file, uint32_t line) {
	log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n",expression, message, file, line);
}

#define FL_ASSERT(expr)													\
    {																	\
        if (expr) {														\
        } else {														\
            report_assertion_failure(#expr, "", __FILE__, __LINE__);	\
            __debugbreak();												\
        }																\
    }

#define FL_ASSERT_MSG(expr, message)									    \
    {																	    \
        if (expr) {														    \
        } else {														    \
            report_assertion_failure(#expr, message, __FILE__, __LINE__);	\
            __debugbreak();												    \
        }																    \
    }

#ifdef _DEBUG

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
#else
#define FL_ASSERT(expr)
#define FL_ASSERT_MSG(expr, message)
#define FL_ASSERT_DEBUG(expr)
#endif