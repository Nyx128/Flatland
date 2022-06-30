#pragma once
#include "FLDevice.hpp"

class FLSwapchain {
public:
	FLSwapchain(FLDevice& _device);
	~FLSwapchain();

	FLSwapchain(const FLSwapchain&) = delete;
	FLSwapchain& operator=(const FLSwapchain&) = delete;

	struct SwapchainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

private:
	FLDevice& device;
	VkSwapchainKHR swapchain;

	SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice phyDevice);
	VkSurfaceFormatKHR chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats);
	VkPresentModeKHR choosePresentMode(std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities);

	void createSwapchain();
};
