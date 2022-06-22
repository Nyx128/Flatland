#pragma once
#include "../core/asserts.hpp"
#include "vulkan/vulkan.h"

void checkResult(VkResult result, const char* message);

#define VK_CHECK_RESULT(result, message) checkResult(result, message);
