#pragma once
#include <optional>
#include <memory>
#include "FLInstance.hpp"
#include "../core/FLWindow.hpp"
#include "VMA/vk_mem_alloc.h"

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
	VkQueue& getGraphicsQueue() { return graphicsQueue; }
	VkQueue& getPresentQueue() { return presentQueue; }
	std::shared_ptr<FLWindow> getWindow() { return flWindow; }
	VkInstance& getInstance() { return flInstance.getInstance(); }
	VmaAllocator& getAllocator() { return allocator; }
	VkCommandPool getCommandPool() { return commandPool; }

private:
	VmaAllocator allocator;
	VkSurfaceKHR windowSurface;//put this before flInstance so it gets destroyed first
	FLInstance flInstance;

	VkDevice device;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties deviceProps;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	std::shared_ptr<FLWindow> flWindow;

	VkCommandPool commandPool;

	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	void createDeviceHandle();
	void createVmaAllocator();
	void createCommandPool();
	void createWindowSurface();
};
