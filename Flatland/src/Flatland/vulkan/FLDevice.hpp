#pragma once
#include <optional>
#include "FLInstance.hpp"

class FLDevice {
public:
	FLDevice();
	~FLDevice();

	FLDevice(const FLDevice&) = delete;
	FLDevice& operator=(const FLDevice&) = delete;

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;

		bool isComplete() { return graphicsFamily.has_value(); }
	};

private:
	VkDevice device;
	FLInstance instance;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties deviceProps;

	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};
