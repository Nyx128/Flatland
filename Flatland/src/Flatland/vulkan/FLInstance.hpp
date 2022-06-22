#pragma once
#include "vulkan/vulkan.h"
#include <vector>

class FLInstance {
public:
	FLInstance();
	~FLInstance();

	FLInstance(const FLInstance&) = delete;
	FLInstance& operator=(const FLInstance&) = delete;

	VkInstance& getInstance() {
		return instance;
	}

private:
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDebugUtilsMessengerCreateInfoEXT debugMsgerCreateInfo{};

	bool checkValidationLayerSupport();
	void setupDebugMessenger();
	std::vector<const char*> getRequiredExtensions();
};
