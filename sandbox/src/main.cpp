#include "Flatland.hpp"
#include <stdlib.h>

int main() {
	FL_FATAL("fatal message %d", 69);
	FL_ERROR("error message %d", 69);
	FL_WARN("warning message %d", 69);
	FL_INFO("info message %d", 69);
	FL_DEBUG("debug message %d", 69);
	FL_TRACE("trace message %d", 69);

	FL_ASSERT_MSG(false, "expr is false");
}