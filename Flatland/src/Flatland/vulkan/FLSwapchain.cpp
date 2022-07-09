#include "FLSwapchain.hpp"
#include "../core/logger.hpp"
#include "vulkan_asserts.hpp"
#include <algorithm>
#include <array>

FLSwapchain::FLSwapchain(FLDevice& _device):device(_device){
	FL_TRACE("FLSwapchain constructor called");
	createSwapchain();
	createRenderPass();
	retrieveSwapchainImages();
	createSwapchainImageViews();
	createDepthResources();
}

FLSwapchain::~FLSwapchain(){
	FL_TRACE("FLSwapchain destructor called");
	for (auto framebuffer : swapchainFramebuffers) {
		vkDestroyFramebuffer(device.getDevice(), framebuffer, nullptr);
	}

	vkDestroyRenderPass(device.getDevice(), renderPass, nullptr);

	for (auto& imageView : swapchainImageViews) {
		vkDestroyImageView(device.getDevice(), imageView, nullptr);
	}

	for (int i = 0; i < depthImages.size(); i++) {
		vkDestroyImageView(device.getDevice(), depthImageViews[i], nullptr);
		vkDestroyImage(device.getDevice(), depthImages[i], nullptr);
		vkFreeMemory(device.getDevice(), depthImageMemorys[i], nullptr);
	}
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

	swapchainExtent = chooseSwapExtent(supportDetails.capabilities);
	swapchainPresentMode = choosePresentMode(supportDetails.presentModes);
	swapchainSurfaceFormat = chooseSurfaceFormat(supportDetails.formats);

	swapchainImageFormat = swapchainSurfaceFormat.format;
	swapchainDepthFormat =
		device.findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

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

void FLSwapchain::createRenderPass(){
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = swapchainDepthFormat;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.srcAccessMask = 0;
	dependency.srcStageMask =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstSubpass = 0;
	dependency.dstStageMask =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask =
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	auto result = vkCreateRenderPass(device.getDevice(), &renderPassInfo, nullptr, &renderPass);
	VK_CHECK_RESULT(result, "Failed to create Swapchain render pass");

}

void FLSwapchain::retrieveSwapchainImages(){
	imageCount = 0;
	vkGetSwapchainImagesKHR(device.getDevice(), swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device.getDevice(), swapchain, &imageCount, swapchainImages.data());
}

void FLSwapchain::createSwapchainImageViews(){
	swapchainImageViews.resize(swapchainImages.size());

	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.format = swapchainSurfaceFormat.format;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	for (int i = 0; i < swapchainImageViews.size(); i++) {
		createInfo.image = swapchainImages[i];

		auto result = vkCreateImageView(device.getDevice(), &createInfo, nullptr, &swapchainImageViews[i]);
		VK_CHECK_RESULT(result, "Failed to created image view for swapchain");
	}
}

void FLSwapchain::createDepthResources(){
	size_t imageCount = swapchainImages.size();

	depthImages.resize(imageCount);
	depthImageMemorys.resize(imageCount);
	depthImageViews.resize(imageCount);

	for (int i = 0; i < depthImages.size(); i++) {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = swapchainExtent.width;
		imageInfo.extent.height = swapchainExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = swapchainDepthFormat;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags = 0;

		device.createImage(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImages[i], depthImageMemorys[i]);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = depthImages[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = swapchainDepthFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		auto result = vkCreateImageView(device.getDevice(), &viewInfo, nullptr, &depthImageViews[i]);
		VK_CHECK_RESULT(result, "Failed to created depth image views");
	}
}

void FLSwapchain::createFramebuffers(){
	swapchainFramebuffers.resize(imageCount);

	

	for (int i = 0; i < imageCount; i++) {
		std::array<VkImageView, 2> attachments = { swapchainImageViews[i], depthImageViews[i] };
		
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapchainExtent.width;
		framebufferInfo.height = swapchainExtent.height;
		framebufferInfo.layers = 1;

		auto result = vkCreateFramebuffer(device.getDevice(), &framebufferInfo, nullptr, &swapchainFramebuffers[i]);
		VK_CHECK_RESULT(result, "Failed to create swapchain framebuffers");
	}
}
