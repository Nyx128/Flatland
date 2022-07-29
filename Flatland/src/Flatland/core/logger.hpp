#pragma once

#define LOG_LEVEL_WARN_ENABLED 1
#define LOG_LEVEL_VALIDATION_ENABLED 1
#define LOG_LEVEL_INFO_ENABLED 1
#define LOG_LEVEL_DEBUG_ENABLED 1
#define LOG_LEVEL_TRACE_ENABLED 1

#ifdef NDEBUG
	#define LOG_LEVEL_DEBUG_ENABLED 0
	#define LOG_LEVEL_TRACE_ENABLED 0
#endif

enum log_level {
	LOG_LEVEL_FATAL = 0,
	LOG_LEVEL_ERROR = 1,
	LOG_LEVEL_VALIDATION = 2,
	LOG_LEVEL_WARN = 3,
	LOG_LEVEL_INFO = 4,
	LOG_LEVEL_DEBUG = 5,
	LOG_LEVEL_TRACE = 6
};

//all log levels will use this to log
void log_output(log_level level, const char* message, ...);

//Logs a fatal log output
#define FL_FATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#ifndef FL_ERROR
//Logs an error message
#define FL_ERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

#if LOG_LEVEL_WARN_ENABLED == 1
//Logs a warn message
#define FL_WARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define FL_WARN(message, ...)
#endif

#if LOG_LEVEL_VALIDATION_ENABLED == 1
//Logs a warn message
#define FL_VALIDATION(message, ...) log_output(LOG_LEVEL_VALIDATION, message, ##__VA_ARGS__);
#else
#define FL_VALIDATION(message, ...)
#endif

#if LOG_LEVEL_INFO_ENABLED == 1
//Logs a warn message
#define FL_INFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define FL_INFO(message, ...)
#endif

#if LOG_LEVEL_DEBUG_ENABLED == 1
//Logs a warn message
#define FL_DEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
#define FL_DEBUG(message, ...)
#endif

#if LOG_LEVEL_TRACE_ENABLED == 1
//Logs a warn message
#define FL_TRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
#define FL_TRACE(message, ...)
#endif