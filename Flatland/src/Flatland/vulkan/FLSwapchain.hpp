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

	VkFormat getImageFormat() { return swapchainImageFormat; }
	VkFormat getDepthFormat() { return swapchainDepthFormat; }
	VkRenderPass& getRenderPass() { return renderPass; }

private:
	FLDevice& device;
	VkSwapchainKHR swapchain;
	VkRenderPass renderPass;

	VkExtent2D swapchainExtent;
	VkPresentModeKHR swapchainPresentMode;
	VkSurfaceFormatKHR swapchainSurfaceFormat;

	VkFormat swapchainImageFormat;
	VkFormat swapchainDepthFormat;

	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;

	SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice phyDevice);
	VkSurfaceFormatKHR chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats);
	VkPresentModeKHR choosePresentMode(std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities);

	void createSwapchain();
	void createRenderPass();
	void retrieveSwapchainImages();
	void createSwapchainImageViews();
};
