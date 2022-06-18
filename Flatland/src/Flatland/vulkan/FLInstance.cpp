#include "FLInstance.hpp"
#include "vulkan_asserts.hpp"
#include "GLFW/include/glfw3.h"

FLInstance::FLInstance(const char* appName){
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.apiVersion = VK_API_VERSION_1_3;
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Flatland";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pNext = nullptr;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	auto result = vkCreateInstance(&createInfo, nullptr, &instance);
	VK_CHECK_RESULT(result, "Failed to created vulkan instance");
}

FLInstance::~FLInstance(){
	vkDestroyInstance(instance, nullptr);
}
