#include "vulkan_asserts.hpp"

void checkResult(VkResult result, const char* message){
    if (result == VK_SUCCESS) {

    }
    else {
        FL_FATAL(message);
        FL_ASSERT_MSG(false, message);
    }
}
