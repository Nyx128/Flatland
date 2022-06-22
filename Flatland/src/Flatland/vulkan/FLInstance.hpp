#pragma once
#include "vulkan/vulkan.h"
#include <vector>

//yes its a global variable
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

class FLInstance {
public:
	FLInstance();
	~FLInstance();

	FLInstance(const FLInstance&) = delete;
	FLInstance& operator=(const FLInstance&) = delete;

	VkInstance& getInstance() {
		return instance;
	}

	const char* const* getValidationLayerNames() { return validationLayers.data(); }

private:
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};


	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDebugUtilsMessengerCreateInfoEXT debugMsgerCreateInfo{};

	void createInstance();
	bool checkValidationLayerSupport();
	void setupDebugMessenger();
	std::vector<const char*> getRequiredExtensions();
};
