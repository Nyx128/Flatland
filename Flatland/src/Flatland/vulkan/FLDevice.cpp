#include "FLDevice.hpp"
#include "vulkan_asserts.hpp"
#include "GLFW/include/glfw3native.h"
#include <set>
#include <string>

FLDevice::FLDevice(std::shared_ptr<FLWindow> window): flWindow(window) {
	FL_TRACE("FLDevice constructor called");
	createWindowSurface();
	pickPhysicalDevice();
	createDeviceHandle();
}

FLDevice::~FLDevice(){
	FL_TRACE("FLDevice destructor called");
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(flInstance.getInstance(), windowSurface, nullptr);
}

void FLDevice::getFramebufferSize(int* width, int* height){
	glfwGetFramebufferSize(flWindow->getWindowPointer(), width, height);
}

uint32_t FLDevice::getGraphicsQueueIndex(){
	auto indices = findQueueFamilies(physicalDevice);
	return indices.graphicsFamily.value();
}

uint32_t FLDevice::getPresentQueueIndex(){
	auto indices = findQueueFamilies(physicalDevice);
	return indices.presentFamily.value();
}

VkFormat FLDevice::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features){
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (
			tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}
	FL_ASSERT_MSG(false, "Failed to find supported format");
}

void FLDevice::pickPhysicalDevice(){
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(flInstance.getInstance(), &deviceCount, nullptr);
	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	vkEnumeratePhysicalDevices(flInstance.getInstance(), &deviceCount, physicalDevices.data());

	for (const auto& device : physicalDevices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}


	FL_ASSERT_MSG(!(physicalDevice == VK_NULL_HANDLE), "Failed to find suitable GPU");

	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProps);
	FL_INFO("Physical Device Selected: %s", deviceProps.deviceName);

}

bool FLDevice::isDeviceSuitable(VkPhysicalDevice device){
	auto indices = findQueueFamilies(device);

	bool deviceExtensionsSupported = checkDeviceExtensionSupport(device);

	return indices.isComplete() && deviceExtensionsSupported;
}

FLDevice::QueueFamilyIndices FLDevice::findQueueFamilies(VkPhysicalDevice device) {

	QueueFamilyIndices indices;

	uint32_t familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
	std::vector<VkQueueFamilyProperties> familyProps(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, familyProps.data());

	uint32_t i = 0;
	for (const auto& family : familyProps) {
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, windowSurface, &presentSupport);
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		if (presentSupport) {
			indices.presentFamily = i;
		}
		if (indices.isComplete()) { break; }
		i++;
	}
	
	return indices;
}

bool FLDevice::checkDeviceExtensionSupport(VkPhysicalDevice device){
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : extensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void FLDevice::createDeviceHandle(){

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::vector<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (auto& queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = queueFamily;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueInfo);

	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.fillModeNonSolid = VK_TRUE;
	deviceFeatures.wideLines = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = (uniqueQueueFamilies[0] == uniqueQueueFamilies[1]) ? &queueCreateInfos[0] : queueCreateInfos.data();
	createInfo.queueCreateInfoCount = (uniqueQueueFamilies[0] == uniqueQueueFamilies[1]) ? 1 : queueCreateInfos.size();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());

	if (enableValidationLayers) {
		createInfo.ppEnabledLayerNames = flInstance.getValidationLayerNames();
		createInfo.enabledLayerCount = 1;
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	auto result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
	VK_CHECK_RESULT(result, "failed to create Vulkan logical device");

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void FLDevice::createWindowSurface(){
	auto result = glfwCreateWindowSurface(flInstance.getInstance(), flWindow->getWindowPointer(), nullptr, &windowSurface);
	VK_CHECK_RESULT(result, "failed to create vulkan window surface");
}
