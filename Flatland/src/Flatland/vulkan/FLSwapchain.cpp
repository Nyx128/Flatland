#include "FLSwapchain.hpp"
#include "../core/logger.hpp"
#include "vulkan_asserts.hpp"
#include <algorithm>

FLSwapchain::FLSwapchain(FLDevice& _device):device(_device) {
	FL_TRACE("FLSwapchain constructor called");
	createSwapchain();
}

FLSwapchain::~FLSwapchain(){
	FL_TRACE("FLSwapchain destructor called");
	vkDestroySwapchainKHR(device.getDevice(), swapchain, nullptr);
}

FLSwapchain::SwapchainSupportDetails FLSwapchain::querySwapchainSupport(VkPhysicalDevice phyDevice){
	SwapchainSupportDetails details;
	//query capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phyDevice, device.getWindowSurface(), &details.capabilities);

	//get available formats
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(phyDevice, device.getWindowSurface(), &formatCount, nullptr);
	FL_ASSERT_MSG(formatCount != 0, "No available VkSurfaceFormatKHR for swapchain");
	details.formats.resize(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(phyDevice, device.getWindowSurface(), &formatCount, details.formats.data());

	//get present modes
	uint32_t presentCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(phyDevice, device.getWindowSurface(), &presentCount, nullptr);
	FL_ASSERT_MSG(presentCount != 0, "No available VkPresentModeKHR for swapchain");
	details.presentModes.resize(formatCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(phyDevice, device.getWindowSurface(), &presentCount, details.presentModes.data());

	if (!details.formats.empty() && !details.presentModes.empty()) {
		return details;
	}
	FL_ASSERT_MSG(false, "Failed to get adequate swapchain details");
}

VkSurfaceFormatKHR FLSwapchain::chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats){
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];//return the first one if preferences aren't met
}

VkPresentModeKHR FLSwapchain::choosePresentMode(std::vector<VkPresentModeKHR> availablePresentModes){
	for (const auto& presentMode : availablePresentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) { return presentMode; }
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D FLSwapchain::chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities){
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		device.getFramebufferSize(&width, &height);

		VkExtent2D actualExtent{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		//clamp the values so they are in the supported range
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void FLSwapchain::createSwapchain(){
	auto supportDetails = querySwapchainSupport(device.getPhysicalDevice());

	VkExtent2D swapchainExtent = chooseSwapExtent(supportDetails.capabilities);
	VkPresentModeKHR swapchainPresentMode = choosePresentMode(supportDetails.presentModes);
	VkSurfaceFormatKHR swapchainSurfaceFormat = chooseSurfaceFormat(supportDetails.formats);

	uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;

	//0 max image count means image count can be anything
	if (supportDetails.capabilities.maxImageCount > 0 && imageCount > supportDetails.capabilities.maxImageCount) {
		imageCount = supportDetails.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapchainCreateInfo{};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.presentMode = swapchainPresentMode;
	swapchainCreateInfo.imageExtent = swapchainExtent;
	swapchainCreateInfo.imageFormat = swapchainSurfaceFormat.format;
	swapchainCreateInfo.imageColorSpace = swapchainSurfaceFormat.colorSpace;
	swapchainCreateInfo.surface = device.getWindowSurface();
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.minImageCount = imageCount;

	uint32_t queueFamilyIndices[] = { device.getGraphicsQueueIndex() , device.getPresentQueueIndex() };
	if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
		swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	}
	else {
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	swapchainCreateInfo.preTransform = supportDetails.capabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	auto result = vkCreateSwapchainKHR(device.getDevice(), &swapchainCreateInfo, nullptr, &swapchain);
	VK_CHECK_RESULT(result, "Failed to create Swapchain");
}
