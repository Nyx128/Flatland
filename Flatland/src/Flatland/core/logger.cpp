#include "logger.hpp"
#include <stdio.h>
#include <cstdarg>
#include <cstdlib>
#include <string>
#include "color.hpp"

char initialize_logging(){
	return true;
}

void shutdown_logging(){
}

FL_API void log_output(log_level level, const char* message, ...){

	const char* levelTags[6] = { "[FATAL]", "[ERROR]", "[WARN]", "[INFO]", "[DEBUG]", "[TRACE]" };
	const char* colorTags[6] = { "red", "light red", "light yellow", "aqua", "green", "bright white" };

	auto tag = dye::colorize(levelTags[level], colorTags[level]).invert();
	
	va_list arg_list;//get the list of variadic arguments
	va_start(arg_list, message);//use the message as the format
	std::cout << tag << std::flush;//print the colored tag

	std::string printMsg = message;
	printMsg = " " + printMsg;

	vfprintf(stdout, printMsg.c_str(), arg_list);//print the message to stdout
	printf("\n");
	va_end(arg_list);
}
