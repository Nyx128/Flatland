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
	VkFramebuffer& getswapchainFramebuffer(uint32_t _imageIndex) {
		return swapchainFramebuffers[_imageIndex];
	}
	VkExtent2D getswapchainExtent() { return swapchainExtent; }
	VkSwapchainKHR& getHandle() { return swapchain; }

private:
	FLDevice& device;
	VkSwapchainKHR swapchain;
	VkRenderPass renderPass;

	VkExtent2D swapchainExtent;
	VkPresentModeKHR swapchainPresentMode;
	VkSurfaceFormatKHR swapchainSurfaceFormat;

	VkFormat swapchainImageFormat;
	VkFormat swapchainDepthFormat;

	uint32_t imageCount = 0;

	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
	std::vector<VkImage> depthImages;
	std::vector<VkImageView> depthImageViews;
	std::vector<VkDeviceMemory> depthImageMemorys;

	std::vector<VkFramebuffer> swapchainFramebuffers;

	SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice phyDevice);
	VkSurfaceFormatKHR chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats);
	VkPresentModeKHR choosePresentMode(std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities);

	void createSwapchain();
	void createRenderPass();
	void retrieveSwapchainImages();
	void createSwapchainImageViews();
	void createDepthResources();
	void createFramebuffers();
};
