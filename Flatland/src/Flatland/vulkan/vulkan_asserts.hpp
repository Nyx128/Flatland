#pragma once
#include "../core/asserts.hpp"
#include "vulkan/vulkan.h"

void checkResult(VkResult result, const char* message) {
    if (result == VK_SUCCESS) {

    }
    else {
        FL_FATAL(message);
        FL_ASSERT_MSG(false, message);
    }
}

#define VK_CHECK_RESULT(result, message) checkResult(result, message);
