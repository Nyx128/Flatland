#include "logger.hpp"
#include <stdio.h>
#include <cstdarg>
#include <cstdlib>
#include <string>
#include "color.hpp"

void log_output(log_level level, const char* message, ...){

	const char* levelTags[7] = { "[FATAL]", "[ERROR]", "[VALIDATION]","[WARN]", "[INFO]", "[DEBUG]", "[TRACE]" };
	const char* colorTags[7] = { "red", "light red", "light purple", "light yellow", "aqua", "green", "bright white" };

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
