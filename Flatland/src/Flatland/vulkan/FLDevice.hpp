#pragma once
#include <optional>
#include <memory>
#include "FLInstance.hpp"
#include "../core/FLWindow.hpp"

class FLDevice {
public:
	FLDevice(std::shared_ptr<FLWindow> window);
	~FLDevice();

	FLDevice(const FLDevice&) = delete;
	FLDevice& operator=(const FLDevice&) = delete;

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

	const std::vector<const char*> deviceExtensions{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkPhysicalDevice& getPhysicalDevice() { return physicalDevice; }
	VkSurfaceKHR& getWindowSurface() { return windowSurface; }
	void getFramebufferSize(int* width, int* height);
	uint32_t getGraphicsQueueIndex();
	uint32_t getPresentQueueIndex();
	VkDevice& getDevice() { return device; }
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createImage(const VkImageCreateInfo& createInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

private:
	VkSurfaceKHR windowSurface;//put this before flInstance so it gets destroyed first
	FLInstance flInstance;

	VkDevice device;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties deviceProps;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	std::shared_ptr<FLWindow> flWindow;


	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	void createDeviceHandle();
	void createWindowSurface();
};
