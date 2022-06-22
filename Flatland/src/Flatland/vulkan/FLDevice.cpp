#include "FLDevice.hpp"
#include "vulkan_asserts.hpp"

FLDevice::FLDevice() {
	FL_TRACE("FLDevice constructor called");
	pickPhysicalDevice();
	createDeviceHandle();
}

FLDevice::~FLDevice(){
	FL_TRACE("FLDevice destructor called");
	vkDestroyDevice(device, nullptr);
}

void FLDevice::pickPhysicalDevice(){
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance.getInstance(), &deviceCount, nullptr);
	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	vkEnumeratePhysicalDevices(instance.getInstance(), &deviceCount, physicalDevices.data());

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

	return indices.isComplete();
}

FLDevice::QueueFamilyIndices FLDevice::findQueueFamilies(VkPhysicalDevice device) {

	QueueFamilyIndices indices;

	uint32_t familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
	std::vector<VkQueueFamilyProperties> familyProps(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, familyProps.data());

	uint32_t i = 0;
	for (const auto& family : familyProps) {
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		if (indices.isComplete()) { break; }
		i++;
	}
	
	return indices;
}

void FLDevice::createDeviceHandle(){

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	VkDeviceQueueCreateInfo queueInfo{};
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.fillModeNonSolid = VK_TRUE;
	deviceFeatures.wideLines = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;

	if (enableValidationLayers) {
		createInfo.ppEnabledLayerNames = instance.getValidationLayerNames();
		createInfo.enabledLayerCount = 1;
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	auto result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
	VK_CHECK_RESULT(result, "failed to create Vulkan logical device");

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
}
