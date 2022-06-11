#include "logger.hpp"
#include <stdio.h>
#include <cstdarg>
#include <cstdlib>

char initialize_logging(){
	return true;
}

void shutdown_logging(){
}

FL_API void log_output(log_level level, const char* message, ...){

	const char* levelTags[6] = { "[FATAL] ", "[ERROR] ", "[WARN] ", "[INFO] ", "[DEBUG] ", "[TRACE] " };

	va_list arg_list;//get the list of variadic arguments
	va_start(arg_list, message);//use the message as the format
	printf(levelTags[level]);
	vfprintf(stdout, message, arg_list);//print the message to stdout
	printf("\n");
	va_end(arg_list);
}
