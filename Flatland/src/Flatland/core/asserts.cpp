#include "asserts.hpp"

void report_assertion_failure(const char* expression, const char* message, const char* file, uint32_t line){
	log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}
